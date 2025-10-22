/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright 2013-2017 CERN
 * Copyright The KiCad Developers, see AUTHORS.txt for contributors.
 *
 * @author Maciej Suminski <maciej.suminski@cern.ch>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#include "gal/include/cached_container_ram.hxx"
#include "gal/include/vertex_manager.hxx"
#include "gal/include/vertex_item.hxx"
#include "gal/include/shader.hxx"
#include "gal/include/utils.hxx"

#include <confirm.hxx>
#include <list>
#include <cassert>

#include <spdlog/spdlog.h>
#ifdef KICAD_GAL_PROFILE
#include <core/profile.h>
#endif /* KICAD_GAL_PROFILE */

using namespace KIGFX;


/**
 * Flag to enable debug output of the GAL OpenGL cached container.
 *
 * Use "KICAD_GAL_CACHED_CONTAINER" to enable GAL OpenGL cached container tracing.
 *
 * @ingroup trace_env_vars
 */
static const std::string const traceGalCachedContainer = "KICAD_GAL_CACHED_CONTAINER";


CACHED_CONTAINER_RAM::CACHED_CONTAINER_RAM( unsigned int aSize ) :
        CACHED_CONTAINER( aSize ),
        m_verticesBuffer( 0 )
{
    m_buffer = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    checkGlError( "generating vertices buffer", __FILE__, __LINE__ );

    m_vertices = static_cast<VERTEX*>( malloc( aSize * VERTEX_SIZE ) );

    if( !m_vertices )
        throw std::bad_alloc();
}


CACHED_CONTAINER_RAM::~CACHED_CONTAINER_RAM()
{


    free( m_vertices );
}


void CACHED_CONTAINER_RAM::Unmap()
{
    if( !m_dirty )
        return;
    QOpenGLFunctions_3_3_Core* function = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_3_3_Core>(QOpenGLContext::currentContext());

    // Upload vertices coordinates and shader types to GPU memory
    m_buffer.bind();
    checkGlError( "binding vertices buffer", __FILE__, __LINE__ );
    m_buffer.setUsagePattern(QOpenGLBuffer::StreamDraw);
    m_buffer.allocate(m_vertices, m_maxIndex * VERTEX_SIZE);
    function->glBufferData( GL_ARRAY_BUFFER, m_maxIndex * VERTEX_SIZE, m_vertices, GL_STREAM_DRAW );
    checkGlError( "transferring vertices", __FILE__, __LINE__ );
    m_buffer.release();
    function->glBindBuffer( GL_ARRAY_BUFFER, 0 );
    checkGlError( "unbinding vertices buffer", __FILE__, __LINE__ );
}


bool CACHED_CONTAINER_RAM::defragmentResize( unsigned int aNewSize )
{
    spdlog::trace("{} Resizing & defragmenting container (memcpy) from {} to {}", traceGalCachedContainer, m_currentSize, aNewSize );

    // No shrinking if we cannot fit all the data
    if( usedSpace() > aNewSize )
        return false;

#ifdef KICAD_GAL_PROFILE
    PROF_TIMER totalTime;
#endif /* KICAD_GAL_PROFILE */

    VERTEX* newBufferMem = static_cast<VERTEX*>( malloc( aNewSize * VERTEX_SIZE ) );

    if( !newBufferMem )
        throw std::bad_alloc();

    defragment( newBufferMem );

    // Switch to the new vertex buffer
    free( m_vertices );
    m_vertices = newBufferMem;

#ifdef KICAD_GAL_PROFILE
    totalTime.Stop();

    wxLogTrace( traceGalCachedContainer, "Defragmented container storing %d vertices / %.1f ms",
                m_currentSize - m_freeSpace, totalTime.msecs() );
#endif /* KICAD_GAL_PROFILE */

    m_freeSpace += ( aNewSize - m_currentSize );
    m_currentSize = aNewSize;

    // Now there is only one big chunk of free memory
    m_freeChunks.clear();
    m_freeChunks.insert( std::make_pair( m_freeSpace, m_currentSize - m_freeSpace ) );
    m_dirty = true;

    return true;
}
