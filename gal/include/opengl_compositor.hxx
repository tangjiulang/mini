/**
 * @file opengl_compositor.h
 * Handle multitarget rendering (ie. to different textures/surfaces) and later compositing
 * into a single image (OpenGL flavor).
 */

#ifndef OPENGL_COMPOSITOR_H_
#define OPENGL_COMPOSITOR_H_

#include <gal/include/compositor.hxx>
#include <gal/include/gal_display_options.hxx>
#include <gal/include/antialiasing.hxx>
#include <deque>

namespace KIGFX
{

class OPENGL_COMPOSITOR : public COMPOSITOR
{
public:
    OPENGL_COMPOSITOR();
    virtual ~OPENGL_COMPOSITOR();

    /// @copydoc COMPOSITOR::Initialize()
    virtual void Initialize() override;

    /// @copydoc COMPOSITOR::Resize()
    virtual void Resize( unsigned int aWidth, unsigned int aHeight ) override;

    /// @copydoc COMPOSITOR::CreateBuffer()
    virtual unsigned int CreateBuffer() override;

    /// @copydoc COMPOSITOR::SetBuffer()
    virtual void SetBuffer( unsigned int aBufferHandle ) override;

    /// @copydoc COMPOSITOR::GetBuffer()
    inline virtual unsigned int GetBuffer() const override
    {
        if( m_curFbo == DIRECT_RENDERING )
            return DIRECT_RENDERING;

        return m_curBuffer + 1;
    }

    /// @copydoc COMPOSITOR::ClearBuffer()
    virtual void ClearBuffer( const COLOR4D& aColor ) override;

    /// @copydoc COMPOSITOR::DrawBuffer()
    virtual void DrawBuffer( unsigned int aBufferHandle ) override;

    /// @copydoc COMPOSITOR::Begin()
    virtual void Begin() override;

    // @copydoc COMPOSITOR::Present()
    virtual void Present() override;

    // Constant used by glBindFramebuffer to turn off rendering to framebuffers
    static const unsigned int DIRECT_RENDERING = 0;

    VECTOR2I GetScreenSize() const;
    GLenum   GetBufferTexture( unsigned int aBufferHandle );
    void     DrawBuffer( unsigned int aSourceHandle, unsigned int aDestHandle );
    unsigned int CreateBuffer( VECTOR2I aDimensions );

    void SetAntialiasingMode( GAL_ANTIALIASING_MODE aMode ); // clears all buffers
    GAL_ANTIALIASING_MODE GetAntialiasingMode() const;

    int GetAntialiasSupersamplingFactor() const;
    VECTOR2D GetAntialiasRenderingOffset() const;

protected:
    /// Binds a specific Framebuffer Object.
    void bindFb( unsigned int aFb );

    /**
     * Perform freeing of resources.
     */
    void clean();

    /// Returns number of used buffers
    inline unsigned int usedBuffers()
    {
        return m_buffers.size();
    }

    // Buffers are simply textures storing a result of certain target rendering.
    struct OPENGL_BUFFER
    {
        VECTOR2I dimensions;
        GLuint textureTarget;                ///< Main texture handle
        GLuint attachmentPoint;              ///< Point to which an image from texture is attached
    };

    bool            m_initialized;            ///< Initialization status flag
    unsigned int    m_curBuffer;              ///< Currently used buffer handle
    GLuint          m_mainFbo;                ///< Main FBO handle (storing all target textures)
    GLuint          m_depthBuffer;            ///< Depth buffer handle
    typedef std::deque<OPENGL_BUFFER> OPENGL_BUFFERS;

    /// Stores information about initialized buffers
    OPENGL_BUFFERS  m_buffers;

    /// Store the used FBO name in case there was more than one compositor used
    GLuint          m_curFbo;

    GAL_ANTIALIASING_MODE m_currentAntialiasingMode;
    std::unique_ptr<OPENGL_PRESENTOR> m_antialiasing;
};
} // namespace KIGFX

#endif /* COMPOSITOR_H_ */
