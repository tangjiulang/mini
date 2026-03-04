/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright The KiCad Developers, see AUTHORS.TXT for contributors.
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

#include <vector>
#include <map>
#include <format>

#include "gal/include/cursors.hxx"
//#include <kiplatform/ui.h>


static const std::map<KICURSOR, std::vector<CURSOR_STORE::CURSOR_DEF>> cursors_defs = {
};


CURSOR_STORE::CURSOR_STORE()
{
    for( const auto& [cursorId, defs] : cursors_defs )
    {
        /*wxCHECK2( !defs.empty(), continue );*/

#if wxCHECK_VERSION( 3, 3, 0 )
        // For wx 3.3+, create cursor bundles from the cursor definitions
        std::vector<wxBitmap> bitmaps;
            
        for( const auto& [xpm, hotspot_def] : defs )
        {
            wxCHECK2( xpm, continue );
            bitmaps.push_back( wxBitmap( xpm ) );
        }

        wxBitmapBundle bitmapBundle = wxBitmapBundle::FromBitmaps( bitmaps );

        wxPoint hotspot = defs[0].m_hotspot; // Use hotspot from standard cursor
        m_bundleMap[cursorId] = wxCursorBundle( bitmapBundle, hotspot );
#else
        auto constructCursor = []( const CURSOR_STORE::CURSOR_DEF& aDef ) -> QCursor
        {
            //wxCHECK( aDef.m_xpm, wxNullCursor );
            QImage xpmImage = QImage( aDef.m_xpm );

            return QCursor( QPixmap::fromImage(xpmImage), aDef.m_hotspot.x(), aDef.m_hotspot.y());
        };

        // Add standard cursor (first definition)
        m_standardCursorMap[cursorId] = constructCursor( defs[0] );

        // Add HiDPI cursor (second definition if available, otherwise fallback to standard)
        if( defs.size() > 1 )
            m_hidpiCursorMap[cursorId] = constructCursor( defs[1] );
        else
            m_hidpiCursorMap[cursorId] = m_standardCursorMap[cursorId];
#endif
    }
}

#if wxCHECK_VERSION( 3, 3, 0 )
const wxCursorBundle& CURSOR_STORE::storeGetBundle( KICURSOR aIdKey ) const
{
    const auto find_iter = m_bundleMap.find( aIdKey );

    if( find_iter != m_bundleMap.end() )
        return find_iter->second;

    wxASSERT_MSG( false, wxString::Format( "Could not find cursor bundle with ID %d",
                                           static_cast<int>( aIdKey ) ) );

    static const wxCursorBundle invalid;

    return invalid;
}
#else
const QCursor& CURSOR_STORE::storeGetCursor( KICURSOR aIdKey, bool aHiDPI ) const
{
    const auto& store = aHiDPI ? m_hidpiCursorMap : m_standardCursorMap;
    const auto  find_iter = store.find( aIdKey );

    if( find_iter != store.end() )
        return find_iter->second;

    assert( false, std::format( "Could not find cursor with ID %d", static_cast<int>( aIdKey ) ) );

    return QCursor(Qt::BlankCursor);
}
#endif

/* static */
const QCURSOR_TYPE CURSOR_STORE::GetCursor( KICURSOR aCursorType, bool aHiDPI )
{
    // Use a single cursor store instance
    static CURSOR_STORE store;

    Qt::CursorShape stock = GetStockCursor( aCursorType );

    if( stock != Qt::ArrowCursor)
        return QCURSOR_TYPE( stock );

#if wxCHECK_VERSION( 3, 3, 0 )
    // For wx 3.3+, return the pre-built cursor bundle (aHiDPI is ignored as bundles contain both)
    return store.storeGetBundle( aCursorType );
#else
    return store.storeGetCursor( aCursorType, aHiDPI );
#endif
}

/* static */
Qt::CursorShape CURSOR_STORE::GetStockCursor( KICURSOR aCursorType )
{
    Qt::CursorShape stockCursor;
    switch( aCursorType )
    {
    case KICURSOR::MOVING:
        stockCursor = Qt::SizeAllCursor;
        break;
    case KICURSOR::BULLSEYE:
        stockCursor = Qt::CrossCursor;
        break;
    case KICURSOR::HAND:
        stockCursor = Qt::PointingHandCursor;
        break;
    case KICURSOR::ARROW:
        stockCursor = Qt::ArrowCursor;
        break;
    default:
        stockCursor = Qt::ArrowCursor;
        break;
    }

    //if( !KIPLATFORM::UI::IsStockCursorOk( stockCursor ) )
    //    stockCursor = wxCURSOR_MAX;

    return stockCursor;
}
