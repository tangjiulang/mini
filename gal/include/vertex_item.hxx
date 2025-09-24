/**
 * @file vertex_item.h
 * Class to handle an item held in a container.
 */

#ifndef VERTEX_ITEM_H_
#define VERTEX_ITEM_H_

#include "vertex_common.hxx"
#include "color4d.hxx"
#include <cstddef>

namespace KIGFX
{
class VERTEX_MANAGER;

class VERTEX_ITEM
{
public:
    friend class CACHED_CONTAINER;
    friend class CACHED_CONTAINER_GPU;
    friend class VERTEX_MANAGER;

    explicit VERTEX_ITEM( const VERTEX_MANAGER& aManager );
    ~VERTEX_ITEM();

    /**
     * Return information about number of vertices stored.
     *
     * @return Number of vertices.
     */
    inline unsigned int GetSize() const
    {
        return m_size;
    }

    /**
     * Return data offset in the container.
     *
     * @return Data offset expressed as a number of vertices.
     */
    inline unsigned int GetOffset() const
    {
        return m_offset;
    }

    /**
     * Return pointer to the data used by the VERTEX_ITEM.
     */
    VERTEX* GetVertices() const;

private:
    /**
     * Set data offset in the container.
     *
     * @param aOffset is the offset expressed as a number of vertices.
     */
    inline void setOffset( unsigned int aOffset )
    {
        m_offset = aOffset;
    }

    /**
     * Set data size in the container.
     *
     * @param aSize is the size expressed as a number of vertices.
     */
    inline void setSize( unsigned int aSize )
    {
        m_size = aSize;
    }

    const VERTEX_MANAGER&   m_manager;
    unsigned int            m_offset;
    unsigned int            m_size;
};
} // namespace KIGFX

#endif /* VERTEX_ITEM_H_ */
