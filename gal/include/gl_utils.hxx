#ifndef GL_UTILS_H
#define GL_UTILS_H

#include <QOpenGLFunctions>
#include <QSurfaceFormat>
#include <GL/wglew.h>
#include <GL/glew.h>

#ifdef _WIN32
    #ifdef __MINGW32__
    #pragma GCC push_options
    #pragma GCC optimize( "O0" )
    #else
    #pragma optimize( "", off )
    #endif
#endif

class GL_UTILS
{
public:
    /**
     * Attempt to set the OpenGL swap interval.
     *
     * @param aVal if -1 = try to set adaptive swapping, 0 = sync off, 1 = sync with VSYNC rate.
     * @return actual value set
     */
    static int SetSwapInterval( int aVal )
    {
#if defined( __linux__ ) && !defined( KICAD_USE_EGL )

        if( Display* dpy = glXGetCurrentDisplay() )
        {
            GLXDrawable drawable = glXGetCurrentDrawable();

            std::string exts( glXQueryExtensionsString( dpy, DefaultScreen( dpy ) ) );

            if( glXSwapIntervalEXT && glXQueryDrawable && drawable
                && exts.find( "GLX_EXT_swap_control" ) != std::string::npos )
            {
                if( aVal == -1 )
                {
                    if( exts.find( "GLX_EXT_swap_control_tear" ) == std::string::npos )
                    {
                        aVal = 1;
                    }
                    else
                    {
                        // Even though the extensions might be available,
                        // we need to be sure that late/adaptive swaps are
                        // enabled on the drawable.

                        unsigned lateSwapsEnabled = 0;
                        glXQueryDrawable( dpy, drawable, GLX_LATE_SWAPS_TEAR_EXT,
                                          &lateSwapsEnabled );

                        if( !lateSwapsEnabled )
                        {
                            aVal = 0;
                        }
                    }
                }

                unsigned clampedInterval;
                glXSwapIntervalEXT( dpy, drawable, aVal );
                glXQueryDrawable( dpy, drawable, GLX_SWAP_INTERVAL_EXT, &clampedInterval );

                return clampedInterval;
            }

            if( glXSwapIntervalMESA && glXGetSwapIntervalMESA
                && exts.find( "GLX_MESA_swap_control" ) != std::string::npos )
            {
                if( aVal == -1 )
                    aVal = 1;

                if( !glXSwapIntervalMESA( aVal ) )
                    return aVal;
            }

            if( glXSwapIntervalSGI && exts.find( "GLX_SGI_swap_control" ) != std::string::npos )
            {
                if( aVal == -1 )
                    aVal = 1;

                if( !glXSwapIntervalSGI( aVal ) )
                    return aVal;
            }
        }

#elif defined( _WIN32 )
        QOpenGLContext* ctx = QOpenGLContext::currentContext();
        if (!ctx) return 0;
        QOpenGLFunctions* function = QOpenGLContext::currentContext()->functions();
        QString vendorStr = QString::fromLatin1(reinterpret_cast<const char*>(function->glGetString(GL_VENDOR)));
        QString versionStr = QString::fromLatin1(reinterpret_cast<const char*>(function->glGetString(GL_VERSION)));
        if (ctx->hasExtension("WGL_EXT_swap_control")) {
            if( aVal == -1 && ( !ctx->hasExtension("WGL_EXT_swap_control_tear")) )
                aVal = 1;

            // Trying to enable adaptive swapping on AMD drivers from 2017 or older leads to crash
            if( aVal == -1 && vendorStr == "ATI Technologies Inc." )
            {
                QStringList parts = versionStr.split('.');

                if( parts.size() == 4 )
                {
                    long majorVer = 0;
                    majorVer = parts[0].toLong();
                    if( majorVer <= 22 )
                        aVal = 1;
                }
            }

            QSurfaceFormat fmt = QSurfaceFormat::defaultFormat();
            fmt.setSwapInterval(aVal);
            QSurfaceFormat::setDefaultFormat(fmt);
            return aVal;
            //HDC   hdc = wglGetCurrentDC();
            //HGLRC hglrc = wglGetCurrentContext();

            //if( hdc && hglrc )
            //{
            //    int currentInterval = wglGetSwapIntervalEXT();

            //    if( currentInterval != aVal )
            //    {
            //        wglSwapIntervalEXT( aVal );
            //        currentInterval = wglGetSwapIntervalEXT();
            //    }

                //return currentInterval;
            //}
        }

#endif
        return 0;
    }
};

#ifdef _WIN32
    #ifdef __MINGW32__
    #pragma GCC pop_options
    #else
    #pragma optimize( "", on )
    #endif
#endif

#endif /* GL_CONTEXT_MANAGER_H */
