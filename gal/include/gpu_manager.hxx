#ifndef GPU_MANAGER_H_
#define GPU_MANAGER_H_

#include <vector>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include "gal/include/vertex_common.hxx"
#include <boost/scoped_array.hpp>

namespace MINI
{
class SHADER;
class VERTEX_CONTAINER;
class VERTEX_ITEM;
class CACHED_CONTAINER;
class NONCACHED_CONTAINER;

/**
 * Class to handle uploading vertices and indices to GPU in drawing purposes.
 */
class GPU_MANAGER
{
public:
    static GPU_MANAGER* MakeManager( VERTEX_CONTAINER* aContainer );

    virtual ~GPU_MANAGER();

    /**
     * Prepare the stored data to be drawn.
     */
    virtual void BeginDrawing() = 0;

    /**
     * Make the GPU draw given range of vertices.
     *
     * @param aOffset is the beginning of the range.
     * @param aSize is the number of vertices to be drawn.
     */
    virtual void DrawIndices( const VERTEX_ITEM* aItem ) = 0;

    /**
     * Clear the container after drawing routines.
     */
    virtual void EndDrawing() = 0;

    /**
     * Allow using shaders with the stored data.
     *
     * @param aShader is the object that allows using shaders.
     */
    virtual void SetShader( SHADER& aShader );

    /**
     * Enable/disable Z buffer depth test.
     */
    void EnableDepthTest( bool aEnabled );

protected:
    GPU_MANAGER( VERTEX_CONTAINER* aContainer );

    ///< Drawing status flag.
    bool m_isDrawing;

    ///< Container that stores vertices data.
    VERTEX_CONTAINER* m_container;

    ///< Shader handling
    SHADER* m_shader;

    ///< Location of shader attributes (for glVertexAttribPointer)
    int m_shaderAttrib;

    ///< true: enable Z test when drawing
    bool m_enableDepthTest;

    GLuint vao = 0, vbo = 0;
};  


class GPU_CACHED_MANAGER : public GPU_MANAGER
{
public:

    struct VRANGE
    {
        VRANGE( int aStart, int aEnd, bool aContinuous ) :
                m_start( aStart ),
                m_end( aEnd ),
                m_isContinuous( aContinuous )
        {
        }

        unsigned int m_start, m_end;
        bool m_isContinuous;
    };


    GPU_CACHED_MANAGER( VERTEX_CONTAINER* aContainer );
    ~GPU_CACHED_MANAGER();

    ///< @copydoc GPU_MANAGER::BeginDrawing()
    virtual void BeginDrawing() override;

    ///< @copydoc GPU_MANAGER::DrawIndices()
    virtual void DrawIndices( const VERTEX_ITEM* aItem ) override;

    ///< @copydoc GPU_MANAGER::EndDrawing()
    virtual void EndDrawing() override;

    ///< Map vertex buffer stored in GPU memory.
    void Map();

    ///< Unmap vertex buffer.
    void Unmap();

protected:
    ///< Resizes the indices buffer to aNewSize if necessary
    void resizeIndices( unsigned int aNewSize );

    ///< Buffers initialization flag
    bool m_buffersInitialized;

    ///< Pointer to the current indices buffer
    boost::scoped_array<GLuint> m_indices;

    ///< Current indices buffer size
    unsigned int m_indicesCapacity;

    ///< Ranges of visible vertex indices to render
    std::vector<VRANGE> m_vranges;

    ///< Number of huge VRANGEs (i.e. large zones) with separate draw calls
    int m_totalHuge;

    ///< Number of regular VRANGEs (small items) pooled into single draw call
    int m_totalNormal;

    ///< Current size of index buffer
    unsigned int m_indexBufSize;

    ///< Maximum size taken by the index buffer for all frames rendered so far
    unsigned int m_indexBufMaxSize;

    ///< Size of the current VRANGE
    unsigned int m_curVrangeSize;

};


class GPU_NONCACHED_MANAGER : public GPU_MANAGER
{
public:
    GPU_NONCACHED_MANAGER( VERTEX_CONTAINER* aContainer );

    ///< @copydoc GPU_MANAGER::BeginDrawing()
    virtual void BeginDrawing() override;

    ///< @copydoc GPU_MANAGER::DrawIndices()
    virtual void DrawIndices( const VERTEX_ITEM* aItem ) override;

    ///< @copydoc GPU_MANAGER::EndDrawing()
    virtual void EndDrawing() override;
};

} // namespace MINI

#endif /* GPU_MANAGER_H_ */
