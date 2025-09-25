#include <list>
#include <spdlog/spdlog.h>
#include <trace_helpers.hxx>
#include "gal/include/cached_container_gpu.hxx"
#include "gal/include/vertex_manager.hxx"
#include "gal/include/vertex_item.hxx"
#include "gal/include/shader.hxx"
#include "gal/include/utils.hxx"
#include "profile.hxx"
//#include "trace_helpers.hxx"

using namespace KIGFX;

/**
 * Flag to enable debug output of the GAL OpenGL GPU cached container.
 *
 * Use "KICAD_GAL_CACHED_CONTAINER_GPU" to enable GAL OpenGL GPU cached container tracing.
 *
 * @ingroup trace_env_vars
 */
static const std::string const traceGalCachedContainerGpu = "KICAD_GAL_CACHED_CONTAINER_GPU";


CACHED_CONTAINER_GPU::CACHED_CONTAINER_GPU( unsigned int aSize ) :
        CACHED_CONTAINER( aSize ),
        m_isMapped( false ),
        m_glBufferHandle( -1 )
{
    m_useCopyBuffer = 0;
    QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();
    QString vendor = QString::fromUtf8((const char*)f->glGetString(GL_VENDOR));
    m_buffer = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    m_buffer.create();

    // workaround for intel GPU drivers:
    // disable glCopyBuffer, causes crashes/freezes on certain driver versions
    // Note, Intel's GL_VENDOR string varies depending on GPU/driver generation
    // But generally always starts with Intel at least
    if( vendor.startsWith( "Intel" ) || vendor.contains( "etnaviv" ) )
    {
        m_useCopyBuffer = false;
    }

    spdlog::trace("{} VBO initial size: %d\n", traceGalProfile.data(), m_currentSize);

    m_buffer.bind();
    m_buffer.allocate(m_currentSize * VERTEX_SIZE);
    m_buffer.release();
    checkGlError( "allocating video memory for cached container", __FILE__, __LINE__ );
}


CACHED_CONTAINER_GPU::~CACHED_CONTAINER_GPU()
{
    if( m_isMapped )
        Unmap();

}


void CACHED_CONTAINER_GPU::Map()
{
    Q_ASSERT(!IsMapped());

    // OpenGL version might suddenly stop being available in Windows when an RDP session is started
    if(!m_buffer.isCreated())
        throw std::runtime_error( "OpenGL no longer available!" );

    m_buffer.bind();
    m_vertices = static_cast<VERTEX*>( m_buffer.map(QOpenGLBuffer::ReadWrite) );

    if( checkGlError( "mapping vertices buffer", __FILE__, __LINE__ ) == GL_NO_ERROR )
        m_isMapped = true;
}


void CACHED_CONTAINER_GPU::Unmap()
{
    Q_ASSERT(IsMapped());

    // This gets called from ~CACHED_CONTAINER_GPU.  To avoid throwing an exception from
    // the dtor, catch it here instead.
    try
    {
        m_buffer.unmap();
        checkGlError( "unmapping vertices buffer", __FILE__, __LINE__ );
        m_buffer.release();
        m_vertices = nullptr;
        checkGlError( "unbinding vertices buffer", __FILE__, __LINE__ );
    }
    catch( const std::runtime_error& err )
    {
        spdlog::trace( "OpenGL did not shut down properly.\n\n{}", err.what() );
    }

    m_isMapped = false;
}


bool CACHED_CONTAINER_GPU::defragmentResize( unsigned int aNewSize )
{
    if( !m_useCopyBuffer )
        return defragmentResizeMemcpy( aNewSize );

    Q_ASSERT(IsMapped());

    spdlog::trace("{} Resizing & defragmenting container from {} to {}", traceGalCachedContainerGpu, m_currentSize,
                aNewSize);

    // No shrinking if we cannot fit all the data
    if( usedSpace() > aNewSize )
        return false;

#ifdef KICAD_GAL_PROFILE
    PROF_TIMER totalTime;
#endif /* KICAD_GAL_PROFILE */


    // glCopyBufferSubData requires a buffer to be unmapped
    m_buffer.unmap();

    // Create a new destination buffer
    QOpenGLBuffer newBuffer(QOpenGLBuffer::VertexBuffer);
    newBuffer.create();
    newBuffer.bind();

    // It would be best to use GL_COPY_WRITE_BUFFER here,
    // but it is not available everywhere
#ifdef KICAD_GAL_PROFILE
    GLint eaBuffer = -1;
    glGetIntegerv( GL_ELEMENT_ARRAY_BUFFER_BINDING, &eaBuffer );
    wxASSERT( eaBuffer == 0 );
#endif /* KICAD_GAL_PROFILE */
    newBuffer.allocate(aNewSize * VERTEX_SIZE);
    checkGlError( "creating buffer during defragmentation", __FILE__, __LINE__ );
    newBuffer.release();

    ITEMS::iterator it, it_end;
    int             newOffset = 0;

    // Defragmentation
    for( it = m_items.begin(), it_end = m_items.end(); it != it_end; ++it )
    {
        VERTEX_ITEM* item = *it;
        int          itemOffset = item->GetOffset();
        int          itemSize = item->GetSize();

        // Move an item to the new container
        VERTEX* src = m_vertices + m_item->GetOffset();
        newBuffer.bind();
        VERTEX* dst = static_cast<VERTEX*>(newBuffer.map(QOpenGLBuffer::WriteOnly));
        std::memcpy(dst + newOffset, src, itemSize * VERTEX_SIZE);
        newBuffer.unmap();
        newBuffer.release();

        // Update new offset
        item->setOffset( newOffset );

        // Move to the next free space
        newOffset += itemSize;
    }

    // Move the current item and place it at the end
    if( m_item->GetSize() > 0 )
    {
        VERTEX* src = m_vertices + m_item->GetOffset();
        newBuffer.bind();
        VERTEX* dst = static_cast<VERTEX*>(newBuffer.map(QOpenGLBuffer::WriteOnly));
        std::memcpy(dst + newOffset, src, m_item->GetSize() * VERTEX_SIZE);
        newBuffer.unmap();
        newBuffer.release();


        m_item->setOffset( newOffset );
        m_chunkOffset = newOffset;
    }

    // Cleanup
    //glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    //glBindBuffer( GL_ARRAY_BUFFER, 0 );

    // Previously we have unmapped the array buffer, now when it is also
    // unbound, it may be officially marked as unmapped
    m_isMapped = false;
    m_buffer.destroy();
    
    // Switch to the new vertex buffer
    m_buffer = newBuffer;
    Map();
    checkGlError( "switching buffers during defragmentation", __FILE__, __LINE__ );

#ifdef KICAD_GAL_PROFILE
    totalTime.Stop();

    wxLogTrace( traceGalCachedContainerGpu, "Defragmented container storing %d vertices / %.1f ms",
                m_currentSize - m_freeSpace, totalTime.msecs() );
#endif /* KICAD_GAL_PROFILE */

    m_freeSpace += ( aNewSize - m_currentSize );
    m_currentSize = aNewSize;

    spdlog::trace("[{}] VBO size {} used {}\n", traceGalProfile.data(), m_currentSize, AllItemsSize());

    // Now there is only one big chunk of free memory
    m_freeChunks.clear();
    m_freeChunks.insert( std::make_pair( m_freeSpace, m_currentSize - m_freeSpace ) );

    return true;
}


bool CACHED_CONTAINER_GPU::defragmentResizeMemcpy( unsigned int aNewSize )
{
    Q_ASSERT( IsMapped());

    spdlog::trace("{} Resizing & defragmenting container (memcpy) from {} to {}", traceGalCachedContainerGpu, m_currentSize,
                aNewSize );

    // No shrinking if we cannot fit all the data
    if( usedSpace() > aNewSize )
        return false;

#ifdef KICAD_GAL_PROFILE
    PROF_TIMER totalTime;
#endif /* KICAD_GAL_PROFILE */

    VERTEX* newBufferMem;

    // Create the destination buffer
    QOpenGLBuffer newBuffer(QOpenGLBuffer::VertexBuffer);

    // It would be best to use GL_COPY_WRITE_BUFFER here,
    // but it is not available everywhere
#ifdef KICAD_GAL_PROFILE
    GLint eaBuffer = -1;
    glGetIntegerv( GL_ELEMENT_ARRAY_BUFFER_BINDING, &eaBuffer );
    wxASSERT( eaBuffer == 0 );
#endif /* KICAD_GAL_PROFILE */

    newBuffer.create();
    newBuffer.bind();
    newBuffer.allocate(aNewSize * VERTEX_SIZE);
    newBufferMem = static_cast<VERTEX*>(newBuffer.map(QOpenGLBuffer::WriteOnly));
    newBuffer.release();
    checkGlError( "creating buffer during defragmentation", __FILE__, __LINE__ );

    defragment( newBufferMem );

    // Cleanup
    Unmap();
    m_buffer.destroy();
    
    // Switch to the new vertex buffer
    m_buffer = newBuffer;
    Map();
    checkGlError( "switching buffers during defragmentation", __FILE__, __LINE__ );

#ifdef KICAD_GAL_PROFILE
    totalTime.Stop();

    wxLogTrace( traceGalCachedContainerGpu, "Defragmented container storing %d vertices / %.1f ms",
                m_currentSize - m_freeSpace, totalTime.msecs() );
#endif /* KICAD_GAL_PROFILE */

    m_freeSpace += ( aNewSize - m_currentSize );
    m_currentSize = aNewSize;

    spdlog::trace("[{}] VBO size {} used: {} \n", traceGalProfile.data(), m_currentSize, AllItemsSize());

    // Now there is only one big chunk of free memory
    m_freeChunks.clear();
    m_freeChunks.insert( std::make_pair( m_freeSpace, m_currentSize - m_freeSpace ) );

    return true;
}


unsigned int CACHED_CONTAINER_GPU::AllItemsSize() const
{
    unsigned int size = 0;

    for( const auto& item : m_items )
    {
        size += item->GetSize();
    }

    return size;
}

