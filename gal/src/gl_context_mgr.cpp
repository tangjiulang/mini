/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2016 CERN
 * Copyright The KiCad Developers, see AUTHORS.txt for contributors.
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

#include "gal/include/gl_context_mgr.hxx"
#include <spdlog/spdlog.h>


QOpenGLContext* GL_CONTEXT_MANAGER::CreateCtx( QOpenGLWidget* aCanvas, const QOpenGLContext* aOther )
{
    QOpenGLContext* context = aCanvas->context();
    Q_ASSERT( context!= nullptr );

    if( !context->isValid() )
    {
        delete context;
        return nullptr;
    }
    m_glContexts.insert( std::make_pair( context, aCanvas ) );

    return context;
}


void GL_CONTEXT_MANAGER::DestroyCtx( QOpenGLContext* aContext )
{
    if( m_glContexts.count( aContext ) )
    {
        m_glContexts.erase( aContext );
        delete aContext;
    }
    else
    {
        // Do not delete unknown GL contexts
        //wxFAIL;
    }

    if( m_glCtx == aContext )
        m_glCtx = nullptr;
}


void GL_CONTEXT_MANAGER::DeleteAll()
{
    m_glCtxMutex.lock();

    for( auto& ctx : m_glContexts )
        delete ctx.first;

    m_glContexts.clear();
    m_glCtx = nullptr;
    m_glCtxMutex.unlock();
}


void GL_CONTEXT_MANAGER::LockCtx(QOpenGLContext* aContext, QOpenGLWidget* aCanvas )
{
    Q_ASSERT( aContext && m_glContexts.count( aContext ) > 0);

    m_glCtxMutex.lock();
    QOpenGLWidget* canvas = aCanvas ? aCanvas : m_glContexts.at( aContext );

    // Prevent assertion failure in wxGLContext::SetCurrent during GAL teardown
#ifdef __WXGTK__

#ifdef KICAD_USE_EGL
    if( canvas->GTKGetDrawingWindow() )
#else
    if( canvas->GetXWindow() )
#endif // KICAD_USE_EGL

#endif // __WXGTK__
    //{
    //    canvas->SetCurrent( *aContext );
    //}

    m_glCtx = aContext;
}


void GL_CONTEXT_MANAGER::UnlockCtx(QOpenGLContext* aContext )
{
    Q_ASSERT( aContext && m_glContexts.count( aContext ) > 0);

    if( m_glCtx == aContext )
    {
        m_glCtxMutex.unlock();
        m_glCtx = nullptr;
    }
    else
    {
        spdlog::trace( std::format( "Trying to unlock GL context mutex from "
                    "a wrong context: aContext {} m_glCtx {}", aContext, m_glCtx ) );
    }
}

