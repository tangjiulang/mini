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
#include <QtGlobal>

#include "gal/include/cursors.hxx"
//#include <kiplatform/ui.h>


static const std::map<KICURSOR, std::vector<CURSOR_STORE::CURSOR_DEF>> cursors_defs = {
};


CURSOR_STORE::CURSOR_STORE()
{
    for( const auto& [cursorId, defs] : cursors_defs )
    {
        /*wxCHECK2( !defs.empty(), continue );*/

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
    }
}

const QCursor& CURSOR_STORE::storeGetCursor( KICURSOR aIdKey, bool aHiDPI ) const
{
    const auto& store = aHiDPI ? m_hidpiCursorMap : m_standardCursorMap;
    const auto  find_iter = store.find( aIdKey );

    if( find_iter != store.end() )
        return find_iter->second;

    //Q_ASSERT( false, std::format( "Could not find cursor with ID %d", static_cast<int>( aIdKey ) ) ); // NOLINT

    return QCursor(Qt::BlankCursor);
}

/* static */
const QCURSOR_TYPE CURSOR_STORE::GetCursor( KICURSOR aCursorType, bool aHiDPI )
{
    // Use a single cursor store instance
    static CURSOR_STORE store;

    Qt::CursorShape stock = GetStockCursor( aCursorType );

    if( stock != Qt::ArrowCursor)
        return QCURSOR_TYPE( stock );

    return store.storeGetCursor( aCursorType, aHiDPI );
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
