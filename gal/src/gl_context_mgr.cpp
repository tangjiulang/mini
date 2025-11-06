#include "gal/include/gl_context_mgr.hxx"
#include <spdlog/spdlog.h>


QOpenGLContext* GL_CONTEXT_MANAGER::CreateCtx( QOpenGLWidget* aCanvas, const QOpenGLContext* aOther )
{
    QOpenGLContext* context = new QOpenGLContext();

    if (aOther)
        context->setShareContext(const_cast<QOpenGLContext*>(aOther));

    if (aCanvas) {
        context->setFormat(static_cast<QOpenGLWidget*>(aCanvas)->format());
    }
    else {
        context->setFormat(QSurfaceFormat::defaultFormat());
    }

    if (!context->create()) {
        qWarning() << "Failed to create QOpenGLContext!";
        delete context;
        return nullptr;
    }
    m_glContexts.insert( std::make_pair( context, aCanvas ) );

    return context;
}


void GL_CONTEXT_MANAGER::DestroyCtx( QOpenGLContext* aContext )
{
    if( m_glContexts.count( aContext ) )
    {
        m_glContexts.erase( aContext );
        delete aContext;
    }
    else
    {
        // Do not delete unknown GL contexts
        //wxFAIL;
    }

    if( m_glCtx == aContext )
        m_glCtx = nullptr;
}


void GL_CONTEXT_MANAGER::DeleteAll()
{
    m_glCtxMutex.lock();

    for( auto& ctx : m_glContexts )
        delete ctx.first;

    m_glContexts.clear();
    m_glCtx = nullptr;
    m_glCtxMutex.unlock();
}


void GL_CONTEXT_MANAGER::LockCtx(QOpenGLContext* aContext, QOpenGLWidget* aCanvas )
{
    if (!(aContext && m_glContexts.count(aContext) > 0)) return;

    m_glCtxMutex.lock();
    QOpenGLWidget* canvas = aCanvas ? aCanvas : m_glContexts.at( aContext );

    // Prevent assertion failure in wxGLContext::SetCurrent during GAL teardown
#ifdef __WXGTK__

#ifdef KICAD_USE_EGL
    if( canvas->GTKGetDrawingWindow() )
#else
    if( canvas->GetXWindow() )
#endif // KICAD_USE_EGL

#endif // __WXGTK__
    //{
    //    canvas->SetCurrent( *aContext );
    //}

    m_glCtx = aContext;
}


void GL_CONTEXT_MANAGER::UnlockCtx(QOpenGLContext* aContext )
{
    if (!(aContext && m_glContexts.count(aContext) > 0)) return;

    if( m_glCtx == aContext )
    {
        m_glCtxMutex.unlock();
        m_glCtx = nullptr;
    }
    else
    {
        spdlog::trace("Trying to unlock GL context mutex from "
                    "a wrong context: aContext {} m_glCtx {}", (int64_t)aContext, (int64_t)m_glCtx );
    }
}

static GL_CONTEXT_MANAGER* contentManager;

GL_CONTEXT_MANAGER* GetGLContextManager() {
    if (contentManager == nullptr)
        contentManager = new GL_CONTEXT_MANAGER;
    return contentManager;
}