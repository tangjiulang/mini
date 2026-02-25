#ifndef CACHED_CONTAINER_RAM_H_
#define CACHED_CONTAINER_RAM_H_

#include <map>
#include <set>
#include "gal/include/cached_container.hxx"

namespace MINI
{
class VERTEX_ITEM;
class SHADER;

/**
 * Specialization of CACHED_CONTAINER that stores data in RAM.
 *
 * This is mainly for video cards/drivers that do not cope well with video memory mapping.
 */

class CACHED_CONTAINER_RAM : public CACHED_CONTAINER
{
public:
    CACHED_CONTAINER_RAM( unsigned int aSize = DEFAULT_SIZE );
    ~CACHED_CONTAINER_RAM();

    ///< @copydoc VERTEX_CONTAINER::Unmap()
    void Map() override {}

    ///< @copydoc VERTEX_CONTAINER::Unmap()
    void Unmap() override;

    bool IsMapped() const override
    {
        return true;
    }

    /**
     * Return handle to the vertex buffer.
     *
     * It might be negative if the buffer is not initialized.
     */
    unsigned int GetBufferHandle() const override
    {
        return m_verticesBuffer;    // make common with CACHED_CONTAINER_RAM
    }

    QOpenGLBuffer* GetBuffer() {
        return &m_buffer;
    }

protected:
    /**
     * Defragment the currently stored data and resizes the buffer.
     *
     * @param aNewSize is the new buffer vertex buffer size, expressed as the number of vertices.
     * @return true on success.
     */
    bool defragmentResize( unsigned int aNewSize ) override;

    ///< Handle to vertices buffer
    GLuint  m_verticesBuffer;
    QOpenGLBuffer m_buffer;
};
} // namespace MINI

#endif /* CACHED_CONTAINER_RAM_H_ */
