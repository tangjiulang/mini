#include "gal/include/gpu_manager.hxx"
#include "gal/include/cached_container_gpu.hxx"
#include "gal/include/cached_container_ram.hxx"
#include "gal/include/noncached_container.hxx"
#include "gal/include/shader.hxx"
#include "gal/include/utils.hxx"
#include "gal/include/vertex_item.hxx"

#include "profile.hxx"
#include "confirm.hxx"
#include "trace_helpers.hxx"
#include <spdlog/spdlog.h>

#include <typeinfo>


#ifdef KICAD_GAL_PROFILE
#include <core/profile.h>
#include <wx/log.h>
#endif /* KICAD_GAL_PROFILE */

using namespace KIGFX;

GPU_MANAGER* GPU_MANAGER::MakeManager( VERTEX_CONTAINER* aContainer )
{
    if( aContainer->IsCached() )
        return new GPU_CACHED_MANAGER( aContainer );
    else
        return new GPU_NONCACHED_MANAGER( aContainer );
}


GPU_MANAGER::GPU_MANAGER( VERTEX_CONTAINER* aContainer ) :
        m_isDrawing( false ),
        m_container( aContainer ),
        m_shader( nullptr ),
        m_shaderAttrib( 0 ),
        m_enableDepthTest( true )
{
}


GPU_MANAGER::~GPU_MANAGER()
{
}


void GPU_MANAGER::SetShader( SHADER& aShader )
{
    m_shader = &aShader;
    m_shaderAttrib = m_shader->GetAttribute( "a_shaderParams" );

    if( m_shaderAttrib == -1 )
    {
        DisplayError( nullptr, "Could not get the shader attribute location");
    }
}


// Cached manager
GPU_CACHED_MANAGER::GPU_CACHED_MANAGER( VERTEX_CONTAINER* aContainer ) :
        GPU_MANAGER( aContainer ),
        m_buffersInitialized( false ),
        m_indicesCapacity( 0 ),
        m_totalHuge( 0 ),
        m_totalNormal( 0 ),
        m_indexBufSize( 0 ),
        m_indexBufMaxSize( 0 ),
        m_curVrangeSize( 0 )
{
    m_vertexArrayObject.create();
}


GPU_CACHED_MANAGER::~GPU_CACHED_MANAGER()
{
}


void GPU_CACHED_MANAGER::BeginDrawing()
{
    Q_ASSERT( !m_isDrawing );

    m_curVrangeSize = 0;
    m_indexBufMaxSize = 0;
    m_indexBufSize = 0;
    m_vranges.clear();

    m_isDrawing = true;
}


void GPU_CACHED_MANAGER::DrawIndices( const VERTEX_ITEM* aItem )
{
    // Hot path: don't use wxASSERT
    assert( m_isDrawing );

    unsigned int offset = aItem->GetOffset();
    unsigned int size = aItem->GetSize();

    if( size == 0 )
        return;

    if( size <= 1000 )
    {
        m_totalNormal += size;
        m_vranges.emplace_back( offset, offset + size - 1, false );
        m_curVrangeSize += size;
    }
    else
    {
        m_totalHuge += size;
        m_vranges.emplace_back( offset, offset + size - 1, true );
        m_indexBufSize = std::max( m_curVrangeSize, m_indexBufSize );
        m_curVrangeSize = 0;
    }
}


void GPU_CACHED_MANAGER::EndDrawing()
{
    Q_ASSERT( m_isDrawing );
    QOpenGLFunctions* function = QOpenGLContext::currentContext()->functions();
    CACHED_CONTAINER* cached = static_cast<CACHED_CONTAINER*>( m_container );

    if( cached->IsMapped() )
        cached->Unmap();

    m_indexBufSize = std::max( m_curVrangeSize, m_indexBufSize );
    m_indexBufMaxSize = std::max( 2*m_indexBufSize, m_indexBufMaxSize );

    resizeIndices( m_indexBufMaxSize );

    if( m_enableDepthTest )
        function->glEnable( GL_DEPTH_TEST );
    else
        function->glDisable( GL_DEPTH_TEST );

    // Prepare buffers
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );

    // Bind vertices data buffers
    QOpenGLBuffer* cachedBuffer = cached->GetBuffer();
    cachedBuffer->bind();
    glVertexPointer( COORD_STRIDE, GL_FLOAT, VERTEX_SIZE, (GLvoid*) COORD_OFFSET );
    glColorPointer( COLOR_STRIDE, GL_UNSIGNED_BYTE, VERTEX_SIZE, (GLvoid*) COLOR_OFFSET );

    if( m_shader != nullptr ) // Use shader if applicable
    {
        m_shader->Use();
        m_shader->program.enableAttributeArray(m_shaderAttrib);
        m_shader->program.setAttributeBuffer(m_shaderAttrib, GL_FLOAT, SHADER_OFFSET, SHADER_STRIDE, VERTEX_SIZE);
        //glVertexAttribPointer( m_shaderAttrib, SHADER_STRIDE, GL_FLOAT, GL_FALSE, VERTEX_SIZE,
        //                       (GLvoid*) SHADER_OFFSET );
    }

    PROF_TIMER cntDraw( "gl-draw-elements" );

    int     n_ranges = m_vranges.size();
    int     n = 0;
    GLuint* iptr = m_indices.get();
    GLuint  icnt = 0;

    int drawCalls = 0;

    while( n < n_ranges )
    {
        VRANGE* cur = &m_vranges[n];

        if( cur->m_isContinuous )
        {
            if( icnt > 0 )
            {
                function->glDrawElements( GL_TRIANGLES, icnt, GL_UNSIGNED_INT, m_indices.get() );
                drawCalls++;
            }

            icnt = 0;
            iptr = m_indices.get();

            function->glDrawArrays( GL_TRIANGLES, cur->m_start, cur->m_end - cur->m_start + 1 );
            drawCalls++;
        }
        else
        {
            for( GLuint i = cur->m_start; i <= cur->m_end; i++ )
            {
                *iptr++ = i;
                icnt++;
            }
        }

        n++;
    }

    if( icnt > 0 )
    {
        function->glDrawElements( GL_TRIANGLES, icnt, GL_UNSIGNED_INT, m_indices.get() );
        drawCalls++;
    }

    cntDraw.Stop();

    spdlog::trace("{} Cached manager size: VBO size {} iranges {} max elt size {} drawcalls {}\n",
        traceGalProfile, cached->AllItemsSize(), m_vranges.size(), m_indexBufMaxSize, drawCalls );
    spdlog::trace( "{} Timing: {}\n", traceGalProfile, cntDraw.to_string() );

    cachedBuffer->release();
    cached->ClearDirty();

    // Deactivate vertex array
    //glDisableClientState( GL_COLOR_ARRAY );
    //glDisableClientState( GL_VERTEX_ARRAY );

    if( m_shader != nullptr )
    {
        m_shader->Deactivate();
    }

    m_isDrawing = false;
}


void GPU_CACHED_MANAGER::resizeIndices( unsigned int aNewSize )
{
    if( aNewSize > m_indicesCapacity )
    {
        m_indicesCapacity = aNewSize;
        m_indices.reset( new GLuint[m_indicesCapacity] );
    }
}


// Noncached manager
GPU_NONCACHED_MANAGER::GPU_NONCACHED_MANAGER( VERTEX_CONTAINER* aContainer ) :
        GPU_MANAGER( aContainer )
{
}


void GPU_NONCACHED_MANAGER::BeginDrawing()
{
    // Nothing has to be prepared
}


void GPU_NONCACHED_MANAGER::DrawIndices( const VERTEX_ITEM* aItem )
{
    Q_ASSERT(false);
}


void GPU_NONCACHED_MANAGER::EndDrawing()
{
#ifdef KICAD_GAL_PROFILE
    PROF_TIMER totalRealTime;
#endif /* KICAD_GAL_PROFILE */
    QOpenGLFunctions* function = QOpenGLContext::currentContext()->functions();
    if( m_container->GetSize() == 0 )
        return;

    VERTEX*  vertices = m_container->GetAllVertices();
    GLfloat* coordinates = (GLfloat*) ( vertices );
    GLubyte* colors = (GLubyte*) ( vertices ) + COLOR_OFFSET;

    if( m_enableDepthTest )
        function->glEnable( GL_DEPTH_TEST );
    else
        function->glDisable( GL_DEPTH_TEST );

    // Prepare buffers
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );

    glVertexPointer( COORD_STRIDE, GL_FLOAT, VERTEX_SIZE, coordinates );
    glColorPointer( COLOR_STRIDE, GL_UNSIGNED_BYTE, VERTEX_SIZE, colors );

    if( m_shader != nullptr ) // Use shader if applicable
    {
        GLfloat* shaders = (GLfloat*) ( vertices ) + SHADER_OFFSET / sizeof( GLfloat );

        m_shader->Use();
        function->glEnableVertexAttribArray( m_shaderAttrib );
        function->glVertexAttribPointer( m_shaderAttrib, SHADER_STRIDE, GL_FLOAT, GL_FALSE, VERTEX_SIZE,
                               shaders );
    }

    function->glDrawArrays( GL_TRIANGLES, 0, m_container->GetSize() );

#ifdef KICAD_GAL_PROFILE
    wxLogTrace( traceGalProfile, wxT( "Noncached manager size: %d" ), m_container->GetSize() );
#endif /* KICAD_GAL_PROFILE */

    // Deactivate vertex array
    glDisableClientState( GL_COLOR_ARRAY );
    glDisableClientState( GL_VERTEX_ARRAY );

    if( m_shader != nullptr )
    {
        function->glDisableVertexAttribArray( m_shaderAttrib );
        m_shader->Deactivate();
    }

    m_container->Clear();

#ifdef KICAD_GAL_PROFILE
    totalRealTime.Stop();
    wxLogTrace( traceGalProfile, wxT( "GPU_NONCACHED_MANAGER::EndDrawing(): %.1f ms" ),
                totalRealTime.msecs() );
#endif /* KICAD_GAL_PROFILE */
}

void GPU_MANAGER::EnableDepthTest( bool aEnabled )
{
    m_enableDepthTest = aEnabled;
}
