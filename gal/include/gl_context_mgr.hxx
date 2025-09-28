#ifndef GL_CONTEXT_MANAGER_H
#define GL_CONTEXT_MANAGER_H

//#include <kicommon.h>
#include <QOpenGLContext>
#include <QOpenGLWidget>
#include <mutex>
#include <map>

class GL_CONTEXT_MANAGER
{
public:

    GL_CONTEXT_MANAGER() : m_glCtx( nullptr ) {}

    /**
     * Create a managed OpenGL context.
     *
     * It is assured that the created context is freed upon exit.  See wxGLContext
     * documentation for the parameters description.
     *
     * @return Created OpenGL context.
     */
    QOpenGLContext* CreateCtx( QOpenGLWidget* aCanvas, const QOpenGLContext* aOther = nullptr );

    /**
     * Destroy a managed OpenGL context.
     *
     * The context to be removed has to be created using GL_CONTEXT_MANAGER::CreateCtx() first.
     *
     * @param aContext is the OpenGL context to be destroyed. It will not be managed anymore.
     */
    void DestroyCtx(QOpenGLContext* aContext );

    /**
     * Destroy all managed OpenGL contexts.
     *
     * This method should be called in the final deinitialization routine.
     */
    void DeleteAll();

    /**
     * Set a context as current and prevents other canvases from switching it.
     *
     * Requires calling UnlockCtx() when there are no more GL calls for the context.  If
     * another canvas has already locked a GL context, then the calling process is blocked.
     *
     * @param aContext is the GL context to be bound.
     * @param aCanvas (optional) allows caller to bind the context to a non-parent canvas
     *                (e.g. when a few canvases share a single GL context).
     */
    void LockCtx(QOpenGLContext* aContext, QOpenGLWidget* aCanvas );

    /**
     * Allow other canvases to bind an OpenGL context.
     *
     * @param aContext is the currently bound context. It is only a check to assure the right
     *                 canvas wants to unlock GL context.
     */
    void UnlockCtx(QOpenGLContext* aContext );

    /**
     * Get the currently bound GL context.
     *
     * @return the currently bound GL context.
     */
    QOpenGLContext* GetCurrentCtx() const
    {
        return m_glCtx;
    }

    /**
     * Get the currently bound GL canvas.
     *
     * @return the currently bound GL canvas.
     */
    QOpenGLWidget* GetCurrentCanvas() const
    {
        auto it = m_glContexts.find( m_glCtx );
        return it != m_glContexts.end() ? it->second : nullptr;
    }

    /**
     * Run the given function first releasing the GL context lock, then restoring it.
     *
     * @param aFunction is the function to be executed.
     */
    template<typename Func, typename... Args>
    auto RunWithoutCtxLock( Func&& aFunction, Args&&... args )
    {
        QOpenGLContext* currentCtx = GetCurrentCtx();
        QOpenGLWidget* currentCanvas = GetCurrentCanvas();
        UnlockCtx( currentCtx );

        if constexpr (std::is_void_v<decltype(aFunction(std::forward<Args>(args)...))>)
        {
            std::forward<Func>(aFunction)(std::forward<Args>(args)...);
            LockCtx( currentCtx, currentCanvas );
            return;
        }
        else
        {
            auto result = std::forward<Func>(aFunction)(std::forward<Args>(args)...);
            LockCtx( currentCtx, currentCanvas );
            return result;
        }
    }

private:
    ///< Map of GL contexts & their parent canvases.
    std::map<QOpenGLContext*, QOpenGLWidget*> m_glContexts;

    ///< Currently bound GL context.
    QOpenGLContext* m_glCtx;

    ///< Lock to prevent unexpected GL context switching.
    std::mutex m_glCtxMutex;
};

GL_CONTEXT_MANAGER* GetGLContextManager();

#endif /* GL_CONTEXT_MANAGER_H */

