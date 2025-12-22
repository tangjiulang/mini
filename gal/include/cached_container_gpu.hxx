#ifndef CACHED_CONTAINER_GPU_H_
#define CACHED_CONTAINER_GPU_H_

#include "gal/include/cached_container.hxx"

namespace KIGFX
{

/**
 * Specialization of CACHED_CONTAINER that stores data in video memory via memory mapping.
 */
class CACHED_CONTAINER_GPU : public CACHED_CONTAINER
{
public:
    CACHED_CONTAINER_GPU( unsigned int aSize = DEFAULT_SIZE );
    ~CACHED_CONTAINER_GPU();

    unsigned int GetBufferHandle() const override
    {
        return m_glBufferHandle;
    }

    QOpenGLBuffer* GetBuffer() {
        return &m_buffer;
    }

    bool IsMapped() const override
    {
        return m_isMapped;
    }

    ///< @copydoc VERTEX_CONTAINER::Map()
    void Map() override;

    ///< @copydoc VERTEX_CONTAINER::Unmap()
    void Unmap() override;

    virtual unsigned int AllItemsSize() const override;


protected:
    /**
     * Remove empty spaces between chunks and optionally resizes the container.
     *
     * After the operation there is continuous space for storing vertices at the end of
     * the container.
     *
     * @param aNewSize is the new size of container, expressed in number of vertices.
     * @return false in case of failure (e.g. memory shortage).
     */
    bool defragmentResize( unsigned int aNewSize ) override;
    bool defragmentResizeMemcpy( unsigned int aNewSize );

    ///< Flag saying if vertex buffer is currently mapped
    bool m_isMapped;

    ///< Vertex buffer handle
    unsigned int m_glBufferHandle;
    QOpenGLBuffer m_buffer;

    ///< Flag saying whether it is safe to use glCopyBufferSubData
    bool m_useCopyBuffer;
};
} // namespace KIGFX

#endif /* CACHED_CONTAINER_GPU_H_ */
