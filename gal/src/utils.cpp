#include <confirm.hxx> // DisplayError

#include <format>
#include <stdexcept>
#include <QOpenGLFunctions>
#include <spdlog/spdlog.h>

/**
 * Flag to enable debug output of the GAL OpenGL error checking.
 *
 * Use "KICAD_GAL_OPENGL_ERROR" to enable GAL OpenGL error tracing.
 *
 * @ingroup trace_env_vars
 */
static const std::string const traceGalOpenGlError = "KICAD_GAL_OPENGL_ERROR";


int checkGlError( const std::string& aInfo, const char* aFile, int aLine, bool aThrow )
{
    QOpenGLFunctions* function = QOpenGLContext::currentContext()->functions();
    int      result = function->glGetError();
    std::string errorMsg;

    switch( result )
    {
    case GL_NO_ERROR:
        // all good
        break;

    case GL_INVALID_ENUM:
        errorMsg = std::format( "Error: {}: invalid enum", aInfo );
        break;

    case GL_INVALID_VALUE:
        errorMsg = std::format( "Error: {}: invalid value", aInfo );
        break;

    case GL_INVALID_OPERATION:
        errorMsg = std::format( "Error: {}: invalid operation", aInfo );
        break;

    case GL_INVALID_FRAMEBUFFER_OPERATION:
    {
        GLenum status = function->glCheckFramebufferStatus( GL_FRAMEBUFFER_EXT );

        if( status != GL_FRAMEBUFFER_COMPLETE_EXT )
        {
            switch( status )
            {
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
                errorMsg = "The framebuffer attachment points are incomplete.";
                break;

            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
                errorMsg = "No images attached to the framebuffer.";
                break;

            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
                errorMsg = "The framebuffer does not have at least one image attached to it.";
                break;

            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
                errorMsg = "The framebuffer read buffer is incomplete.";
                break;

            case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
                errorMsg = "The combination of internal formats of the attached images violates "
                           "an implementation dependent set of restrictions.";
                break;

            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_EXT:
                errorMsg = "GL_RENDERBUFFER_SAMPLES is not the same for all attached render "
                           "buffers.";
                break;

            case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS_EXT:
                errorMsg = "Framebuffer incomplete layer targets errors.";
                break;

            case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
                errorMsg = "Framebuffer attachments have different dimensions";
                break;

            default:
                errorMsg = std::format( "Unknown incomplete framebuffer error id {}", status );
            }
        }
        else
        {
            errorMsg = std::format( "Error: {}: invalid framebuffer operation", aInfo );
        }
    }
    break;

    case GL_OUT_OF_MEMORY:
        errorMsg = std::format( "Error: {}: out of memory", aInfo );
        break;

    case GL_STACK_UNDERFLOW:
        errorMsg = std::format( "Error: {}: stack underflow", aInfo );
        break;

    case GL_STACK_OVERFLOW:
        errorMsg = std::format( "Error: {}: stack overflow", aInfo );
        break;

    default:
        errorMsg = std::format( "Error: {}: unknown error", aInfo );
        break;
    }

    if( result != GL_NO_ERROR )
    {
        if( aThrow )
        {
            spdlog::trace("{} Throwing exception for glGetError() '{}' "
                                                  "in file '{}' on line {}.",
                        traceGalOpenGlError,
                        errorMsg,
                        aFile,
                        aLine );

            throw std::runtime_error( errorMsg.data() );
        }
        else
        {
            std::string msg = std::format("glGetError() '{}' in file '{}' on line {}.", errorMsg, aFile, aLine);

            DisplayErrorMessage( nullptr, "OpenGL Error", errorMsg.data() );
        }
    }

    return result;
}


// debugMsgCallback is a callback function for glDebugMessageCallback.
// It must have the right type ( GLAPIENTRY )
static void debugMsgCallback( GLenum aSource, GLenum aType, GLuint aId, GLenum aSeverity,
                                         GLsizei aLength, const GLchar* aMessage,
                                         const void* aUserParam )
{
    switch( aSeverity )
    {
    case GL_DEBUG_SEVERITY_HIGH:
        spdlog::trace("{} OpenGL ERROR: {}", traceGalOpenGlError, aMessage );
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        spdlog::trace("{} OpenGL WARNING: {}", traceGalOpenGlError, aMessage );
        break;
    case GL_DEBUG_SEVERITY_LOW:
        spdlog::trace("{} OpenGL INFO: {}", traceGalOpenGlError, aMessage );
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        return;
    }
}


void enableGlDebug( bool aEnable )
{
    QOpenGLFunctions* function = QOpenGLContext::currentContext()->functions();
    if( aEnable )
    {
        function->glEnable( GL_DEBUG_OUTPUT );
        //function->glDebugMessageCallback( (GLDEBUGPROC) debugMsgCallback, nullptr );
    }
    else
    {
        function->glDisable( GL_DEBUG_OUTPUT );
    }
}
