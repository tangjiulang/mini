/**
 * @file noncached_container.h
 * @brief Class to store instances of VERTEX without caching. It allows a fast one-frame drawing
 * and then clearing the buffer and starting from scratch.
 */

#ifndef NONCACHED_CONTAINER_H_
#define NONCACHED_CONTAINER_H_

#include "gal/include/vertex_container.hxx"

namespace KIGFX
{
class VERTEX_ITEM;
class SHADER;

class NONCACHED_CONTAINER : public VERTEX_CONTAINER
{
public:
    /**
     * Construct a non-cached container object.
     *
     * @param aSize is the size of the cache.
     * @throw bad_alloc exception if memory allocation fails.
     */
    NONCACHED_CONTAINER( unsigned int aSize = DEFAULT_SIZE );
    virtual ~NONCACHED_CONTAINER();

    bool IsCached() const override
    {
        return false;
    }

    /// @copydoc VERTEX_CONTAINER::SetItem( VERTEX_ITEM* aItem )
    virtual void SetItem( VERTEX_ITEM* aItem ) override;

    /// @copydoc VERTEX_CONTAINER::Allocate( unsigned int aSize )
    virtual VERTEX* Allocate( unsigned int aSize ) override;

    /// @copydoc VERTEX_CONTAINER::Delete( VERTEX_ITEM* aItem )
    void Delete( VERTEX_ITEM* aItem ) override {}

    /// @copydoc VERTEX_CONTAINER::Clear()
    virtual void Clear() override;

    /// @copydoc VERTEX_CONTAINER::GetSize()
    virtual unsigned int GetSize() const override
    {
        // As the m_freePtr points to the first free space, we can safely assume
        // that this is the number of vertices stored inside
        return m_freePtr;
    }

protected:
    ///< Index of the free first space where a vertex can be stored
    unsigned int m_freePtr;
};
} // namespace KIGFX

#endif /* NONCACHED_CONTAINER_H_ */
