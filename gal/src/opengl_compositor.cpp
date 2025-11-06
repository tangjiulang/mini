/**
 * @file opengl_compositor.cpp
 * @brief Class that handles multitarget rendering (i.e. to different textures/surfaces) and
 * later compositing into a single image (OpenGL flavour).
 */

#include <gal/include/opengl_compositor.hxx>
#include <gal/include/utils.hxx>

#include <color4d.hxx>
#include <QOpenGLVersionFunctionsFactory>
#include <cassert>
#include <memory>
#include <stdexcept>
#include <spdlog/spdlog.h>

using namespace KIGFX;

OPENGL_COMPOSITOR::OPENGL_COMPOSITOR() :
        m_initialized( false ),
        m_curBuffer( 0 ),
        m_mainFbo( 0 ),
        m_depthBuffer( 0 ),
        m_curFbo( DIRECT_RENDERING ),
        m_currentAntialiasingMode( GAL_ANTIALIASING_MODE::AA_NONE )
{
    m_antialiasing = std::make_unique<ANTIALIASING_NONE>( this );
}


OPENGL_COMPOSITOR::~OPENGL_COMPOSITOR()
{
    if( m_initialized )
    {
        try
        {
            clean();
        }
        catch( const std::runtime_error& exc )
        {
            spdlog::error("Run time exception `{}` occurred in OPENGL_COMPOSITOR destructor.", exc.what() );
        }
    }
}


void OPENGL_COMPOSITOR::SetAntialiasingMode( GAL_ANTIALIASING_MODE aMode )
{
    m_currentAntialiasingMode = aMode;

    if( m_initialized )
        clean();
}


GAL_ANTIALIASING_MODE OPENGL_COMPOSITOR::GetAntialiasingMode() const
{
    return m_currentAntialiasingMode;
}


void OPENGL_COMPOSITOR::Initialize()
{
    if( m_initialized )
        return;
    QOpenGLFunctions_3_3_Core* function = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_3_3_Core>(QOpenGLContext::currentContext());
    switch( m_currentAntialiasingMode )
    {
    case GAL_ANTIALIASING_MODE::AA_FAST:
        m_antialiasing = std::make_unique<ANTIALIASING_SMAA>( this );
        break;
    case GAL_ANTIALIASING_MODE::AA_HIGHQUALITY:
        m_antialiasing = std::make_unique<ANTIALIASING_SUPERSAMPLING>( this );
        break;
    default:
        m_antialiasing = std::make_unique<ANTIALIASING_NONE>( this );
        break;
    }

    VECTOR2I dims = m_antialiasing->GetInternalBufferSize();
    assert( dims.x != 0 && dims.y != 0 );

    GLint maxBufSize;
    function->glGetIntegerv( GL_MAX_RENDERBUFFER_SIZE_EXT, &maxBufSize );

    if( dims.x < 0 || dims.y < 0 || dims.x > maxBufSize || dims.y >= maxBufSize )
        throw std::runtime_error( "Requested render buffer size is not supported" );

    // We need framebuffer objects for drawing the screen contents
    // Generate framebuffer and a depth buffer
    function->glGenFramebuffers( 1, &m_mainFbo );
    checkGlError( "generating framebuffer", __FILE__, __LINE__ );
    bindFb( m_mainFbo );
    function->glViewport(0, 0, dims.x, dims.y);

    // Allocate memory for the depth buffer
    // Attach the depth buffer to the framebuffer
    function->glGenRenderbuffers( 1, &m_depthBuffer );
    checkGlError( "generating renderbuffer", __FILE__, __LINE__ );
    function->glBindRenderbuffer( GL_RENDERBUFFER_EXT, m_depthBuffer );
    checkGlError( "binding renderbuffer", __FILE__, __LINE__ );

    function->glRenderbufferStorage( GL_RENDERBUFFER_EXT, GL_DEPTH24_STENCIL8, dims.x, dims.y );
    checkGlError( "creating renderbuffer storage", __FILE__, __LINE__ );
    function->glFramebufferRenderbuffer( GL_FRAMEBUFFER_EXT, GL_DEPTH_STENCIL_ATTACHMENT,
                                  GL_RENDERBUFFER_EXT, m_depthBuffer );
    checkGlError( "attaching renderbuffer", __FILE__, __LINE__ );


    GLenum status = function->glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
        qDebug() << "Framebuffer incomplete: " << QString::number(status, 16);

    // Unbind the framebuffer, so by default all the rendering goes directly to the display
    bindFb( DIRECT_RENDERING );

    GLint drawBuf;
    glGetIntegerv(GL_DRAW_BUFFER, &drawBuf);
    qDebug() << "Current draw buffer:" << drawBuf;

    m_initialized = true;

    m_antialiasing->Init();
}


void OPENGL_COMPOSITOR::Resize( unsigned int aWidth, unsigned int aHeight )
{
    if( m_initialized )
        clean();

    m_antialiasing->OnLostBuffers();

    m_width = aWidth;
    m_height = aHeight;
}


unsigned int OPENGL_COMPOSITOR::CreateBuffer()
{
    return m_antialiasing->CreateBuffer();
}


unsigned int OPENGL_COMPOSITOR::CreateBuffer( VECTOR2I aDimensions )
{
    assert( m_initialized );
    QOpenGLFunctions_3_3_Core* function = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_3_3_Core>(QOpenGLContext::currentContext());
    int maxBuffers, maxTextureSize;

    // Get the maximum number of buffers
    function->glGetIntegerv( GL_MAX_COLOR_ATTACHMENTS, (GLint*) &maxBuffers );

    if( (int) usedBuffers() >= maxBuffers )
    {
        throw std::runtime_error( "Cannot create more framebuffers. OpenGL rendering backend requires at "
                                  "least 3 framebuffers. You may try to update/change your graphic drivers." );
    }

    function->glGetIntegerv( GL_MAX_TEXTURE_SIZE, (GLint*) &maxTextureSize );

    if( maxTextureSize < (int) aDimensions.x || maxTextureSize < (int) aDimensions.y )
    {
        throw std::runtime_error( "Requested texture size is not supported. Could not create a buffer." );
    }

    // GL_COLOR_ATTACHMENTn are consecutive integers
    GLuint attachmentPoint = GL_COLOR_ATTACHMENT0 + usedBuffers();
    GLuint textureTarget;

    // Generate the texture for the pixel storage
    function->glActiveTexture( GL_TEXTURE0 );
    function->glGenTextures( 1, &textureTarget );
    checkGlError( "generating framebuffer texture target", __FILE__, __LINE__ );
    function->glBindTexture( GL_TEXTURE_2D, textureTarget );
    checkGlError( "binding framebuffer texture target", __FILE__, __LINE__ );

    // Set texture parameters
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    function->glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, aDimensions.x, aDimensions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr );
    checkGlError( "creating framebuffer texture", __FILE__, __LINE__ );
    function->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    function->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

    // Bind the texture to the specific attachment point, clear and rebind the screen
    bindFb( m_mainFbo );
    function->glFramebufferTexture2D( GL_FRAMEBUFFER_EXT, attachmentPoint, GL_TEXTURE_2D, textureTarget, 0 );

    // Check the status, exit if the framebuffer can't be created
    GLenum status = function->glCheckFramebufferStatus( GL_FRAMEBUFFER_EXT );

    if( status != GL_FRAMEBUFFER_COMPLETE_EXT )
    {
        switch( status )
        {
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
            throw std::runtime_error( "The framebuffer attachment points are incomplete." );

        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
            throw std::runtime_error( "No images attached to the framebuffer." );

        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
            throw std::runtime_error( "The framebuffer does not have at least one image attached to it." );

        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
            throw std::runtime_error( "The framebuffer read buffer is incomplete." );

        case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
            throw std::runtime_error( "The combination of internal formats of the attached images violates "
                                      "an implementation-dependent set of restrictions." );

        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_EXT:
            throw std::runtime_error( "GL_RENDERBUFFER_SAMPLES is not the same for all attached renderbuffers" );

        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS_EXT:
            throw std::runtime_error( "Framebuffer incomplete layer targets errors." );

        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
            throw std::runtime_error( "Framebuffer attachments have different dimensions" );

        default:
            throw std::runtime_error( "Unknown error occurred when creating the framebuffer." );
        }
    }

    ClearBuffer( COLOR4D::BLACK );

    // Return to direct rendering (we were asked only to create a buffer, not switch to one)
    bindFb( DIRECT_RENDERING );

    // Store the new buffer
    OPENGL_BUFFER buffer = { aDimensions, textureTarget, attachmentPoint };
    m_buffers.push_back( buffer );

    return usedBuffers();
}


GLenum OPENGL_COMPOSITOR::GetBufferTexture( unsigned int aBufferHandle )
{
    if ( aBufferHandle > 0 && aBufferHandle <= usedBuffers()) return 0;
    return m_buffers[aBufferHandle - 1].textureTarget;
}


void OPENGL_COMPOSITOR::SetBuffer( unsigned int aBufferHandle )
{
    if ( m_initialized && aBufferHandle <= usedBuffers()) return;
    QOpenGLFunctions_3_3_Core* function = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_3_3_Core>(QOpenGLContext::currentContext());

    // Either unbind the FBO for direct rendering, or bind the one with target textures
    bindFb( aBufferHandle == DIRECT_RENDERING ? DIRECT_RENDERING : m_mainFbo );

    // Switch the target texture
    if( m_curFbo != DIRECT_RENDERING )
    {
        m_curBuffer = aBufferHandle - 1;
        function->glDrawBuffer( m_buffers[m_curBuffer].attachmentPoint );
        checkGlError( "setting draw buffer", __FILE__, __LINE__ );

        function->glViewport( 0, 0, m_buffers[m_curBuffer].dimensions.x, m_buffers[m_curBuffer].dimensions.y );
    }
    else
    {
        function->glViewport( 0, 0, GetScreenSize().x, GetScreenSize().y );
    }
}


void OPENGL_COMPOSITOR::ClearBuffer( const COLOR4D& aColor )
{
    if( m_initialized) return;
    QOpenGLFunctions_3_3_Core* function = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_3_3_Core>(QOpenGLContext::currentContext());

    function->glClearColor( aColor.r, aColor.g, aColor.b, m_curFbo == DIRECT_RENDERING ? 1.0f : 0.0f );
    function->glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
}


VECTOR2I OPENGL_COMPOSITOR::GetScreenSize() const
{
    typedef VECTOR2I::coord_type coord_t;
    Q_ASSERT( m_width <= static_cast<unsigned int>( std::numeric_limits<coord_t>::max() ) );
    Q_ASSERT( m_height <= static_cast<unsigned int>( std::numeric_limits<coord_t>::max() ) );

    return { static_cast<coord_t>( m_width ), static_cast<coord_t>( m_height ) };
}


void OPENGL_COMPOSITOR::Begin()
{
    m_antialiasing->Begin();
}


void OPENGL_COMPOSITOR::DrawBuffer( unsigned int aBufferHandle )
{
    m_antialiasing->DrawBuffer( aBufferHandle );
}


void OPENGL_COMPOSITOR::DrawBuffer( unsigned int aSourceHandle, unsigned int aDestHandle )
{
    if ( m_initialized && aSourceHandle != 0 && aSourceHandle <= usedBuffers()) return;
    if( aDestHandle <= usedBuffers()) return;
    QOpenGLFunctions_3_3_Core* function = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_3_3_Core>(QOpenGLContext::currentContext());
    // Switch to the destination buffer and blit the scene
    SetBuffer( aDestHandle );

    // Depth test has to be disabled to make transparency working
    function->glDisable( GL_DEPTH_TEST );
    function->glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );

    // Enable texturing and bind the main texture
    function->glEnable( GL_TEXTURE_2D );
    function->glBindTexture( GL_TEXTURE_2D, m_buffers[aSourceHandle - 1].textureTarget );

    // Draw a full screen quad with the texture
    //glMatrixMode( GL_MODELVIEW );
    //glPushMatrix();
    //glLoadIdentity();
    //glMatrixMode( GL_PROJECTION );
    //glPushMatrix();
    //glLoadIdentity();

    //glBegin( GL_TRIANGLES );
    //glTexCoord2f( 0.0f, 1.0f );
    //glVertex2f( -1.0f, 1.0f );
    //glTexCoord2f( 0.0f, 0.0f );
    //glVertex2f( -1.0f, -1.0f );
    //glTexCoord2f( 1.0f, 1.0f );
    //glVertex2f( 1.0f, 1.0f );

    //glTexCoord2f( 1.0f, 1.0f );
    //glVertex2f( 1.0f, 1.0f );
    //glTexCoord2f( 0.0f, 0.0f );
    //glVertex2f( -1.0f, -1.0f );
    //glTexCoord2f( 1.0f, 0.0f );
    //glVertex2f( 1.0f, -1.0f );
    //glEnd();

    //glPopMatrix();
    //glMatrixMode( GL_MODELVIEW );
    //glPopMatrix();
}


void OPENGL_COMPOSITOR::Present()
{
    m_antialiasing->Present();
}


void OPENGL_COMPOSITOR::bindFb( unsigned int aFb )
{
    QOpenGLFunctions_3_3_Core* function = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_3_3_Core>(QOpenGLContext::currentContext());
    // Currently there are only 2 valid FBOs
    Q_ASSERT( aFb == DIRECT_RENDERING || aFb == m_mainFbo );

    if( m_curFbo != aFb )
    {
        function->glBindFramebuffer( GL_FRAMEBUFFER, aFb );
        checkGlError( "switching framebuffer", __FILE__, __LINE__ );
        m_curFbo = aFb;
    }
}


void OPENGL_COMPOSITOR::clean()
{
    if ( m_initialized) return;

    bindFb( DIRECT_RENDERING );

    QOpenGLFunctions_3_3_Core* function = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_3_3_Core>(QOpenGLContext::currentContext());
    for( const OPENGL_BUFFER& buffer : m_buffers )
        function->glDeleteTextures( 1, &buffer.textureTarget );

    m_buffers.clear();

    function->glDeleteFramebuffers( 1, &m_mainFbo );

    function->glDeleteRenderbuffers( 1, &m_depthBuffer );

    m_initialized = false;
}


int OPENGL_COMPOSITOR::GetAntialiasSupersamplingFactor() const
{
    switch ( m_currentAntialiasingMode )
    {
    case GAL_ANTIALIASING_MODE::AA_HIGHQUALITY: return 2;
    default:                                      return 1;
    }
}

VECTOR2D OPENGL_COMPOSITOR::GetAntialiasRenderingOffset() const
{
    switch( m_currentAntialiasingMode )
    {
    case GAL_ANTIALIASING_MODE::AA_HIGHQUALITY: return VECTOR2D( 0.5, -0.5 );
    default:                                      return VECTOR2D( 0, 0 );
    }
}
