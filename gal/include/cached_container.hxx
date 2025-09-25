#ifndef CACHED_CONTAINER_H_
#define CACHED_CONTAINER_H_

#include <map>
#include <set>
#include <QOpenGLBuffer>

#include "vertex_container.hxx"

namespace KIGFX
{
class VERTEX_ITEM;
class SHADER;

/**
 * Class to store VERTEX instances with caching.
 *
 * It associates VERTEX objects and with VERTEX_ITEMs. Caching vertices data in the memory and a
 * enables fast reuse of that data.
 */

class CACHED_CONTAINER : public VERTEX_CONTAINER
{
public:
    CACHED_CONTAINER( unsigned int aSize = DEFAULT_SIZE );
    virtual ~CACHED_CONTAINER() {}

    bool IsCached() const override
    {
        return true;
    }

    virtual void SetItem( VERTEX_ITEM* aItem ) override;

    ///< @copydoc VERTEX_CONTAINER::FinishItem()
    virtual void FinishItem() override;

    /**
     * Return allocated space for the requested number of vertices associated with the
     * current item (set with SetItem()).
     *
     * The allocated space is added at the end of the chunk used by the current item and
     * may serve to store new vertices.
     *
     * @param aSize is the number of vertices to be allocated.
     * @return Pointer to the allocated space.
     * @throw bad_alloc exception if allocation fails.
     */
    virtual VERTEX* Allocate( unsigned int aSize ) override;

    ///< @copydoc VERTEX_CONTAINER::Delete()
    virtual void Delete( VERTEX_ITEM* aItem ) override;

    ///< @copydoc VERTEX_CONTAINER::Clear()
    virtual void Clear() override;

    /**
     * Return handle to the vertex buffer. It might be negative if the buffer is not initialized.
     */
    virtual unsigned int GetBufferHandle() const = 0;
    virtual QOpenGLBuffer* GetBuffer() = 0;

    /**
     * Return true if vertex buffer is currently mapped.
     */
    virtual bool IsMapped() const = 0;

    ///< @copydoc VERTEX_CONTAINER::Map()
    virtual void Map() override = 0;

    ///< @copydoc VERTEX_CONTAINER::Unmap()
    virtual void Unmap() override = 0;

    virtual unsigned int AllItemsSize() const { return 0; }

protected:
    ///< Maps size of free memory chunks to their offsets
    typedef std::pair<unsigned int, unsigned int> CHUNK;
    typedef std::multimap<unsigned int, unsigned int> FREE_CHUNK_MAP;

    /// List of all the stored items
    typedef std::set<VERTEX_ITEM*> ITEMS;

    /**
     * Resize the chunk that stores the current item to the given size. The current item has
     * its offset adjusted after the call, and the new chunk parameters are stored
     * in m_chunkOffset and m_chunkSize.
     *
     * @param aSize is the requested chunk size.
     * @return true in case of success, false otherwise.
     */
    bool reallocate( unsigned int aSize );

    /**
     * Remove empty spaces between chunks and optionally resizes the container.
     *
     * After the operation there is continuous space for storing vertices at the end of the
     * container.
     *
     * @param aNewSize is the new size of container, expressed in number of vertices.
     * @return false in case of failure (e.g. memory shortage).
     */
    virtual bool defragmentResize( unsigned int aNewSize ) = 0;

    /**
     * Transfer all stored data to a new buffer, removing empty spaces between the data chunks
     * in the container.
     *
     * @param aTarget is the destination for the defragmented data.
     */
    void defragment( VERTEX* aTarget );

    /**
     * Look for consecutive free memory chunks and merges them, decreasing fragmentation of
     * memory.
     */
    void mergeFreeChunks();

    /**
     * Return the size of a chunk.
     *
     * @param aChunk is the chunk.
     */
    inline int getChunkSize( const CHUNK& aChunk ) const
    {
        return aChunk.first;
    }

    /**
     * Return the offset of a chunk.
     *
     * @param aChunk is the chunk.
     */
    inline unsigned int getChunkOffset( const CHUNK& aChunk ) const
    {
        return aChunk.second;
    }

    /**
     * Add a chunk marked as a free space.
     */
    void addFreeChunk( unsigned int aOffset, unsigned int aSize );

    ///< Store size & offset of free chunks.
    FREE_CHUNK_MAP  m_freeChunks;

    ///< Stored VERTEX_ITEMs
    ITEMS m_items;

    ///< Currently modified item
    VERTEX_ITEM* m_item;

    ///< Properties of currently modified chunk & item
    unsigned int m_chunkSize;
    unsigned int m_chunkOffset;

    ///< Maximal vertex index number stored in the container
    unsigned int m_maxIndex;

private:
    /// Debug & test functions
    void showFreeChunks();
    void showUsedChunks();
    void test();
};
} // namespace KIGFX

#endif /* CACHED_CONTAINER_H_ */
