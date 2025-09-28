#ifndef CURSOR_STORE__H
#define CURSOR_STORE__H
#include <QCursor>

#include <map>
#include <vector>

/**
 * Represents either a wxCursorBundle for wx 3.3+ or a wxCursor for older versions.
 * This allows consistent cursor handling across different wxWidgets versions.
 */
#if wxCHECK_VERSION( 3, 3, 0 )
typedef wxCursorBundle WX_CURSOR_TYPE;
#else
typedef QCursor QCURSOR_TYPE;
#endif


enum class KICURSOR
{
    DEFAULT,
    ARROW,
    ARROW64,
    MOVING,
    MOVING64,
    PENCIL,
    PENCIL64,
    REMOVE,
    REMOVE64,
    HAND,
    HAND64,
    BULLSEYE,
    BULLSEYE64,
    VOLTAGE_PROBE,
    VOLTAGE_PROBE64,
    CURRENT_PROBE,
    CURRENT_PROBE64,
    TUNE,
    TUNE64,
    TEXT,
    TEXT64,
    MEASURE,
    MEASURE64,
    ADD,
    ADD64,
    SUBTRACT,
    SUBTRACT64,
    XOR,
    XOR64,
    ZOOM_IN,
    ZOOM_IN64,
    ZOOM_OUT,
    ZOOM_OUT64,
    LABEL_NET,
    LABEL_NET64,
    LABEL_GLOBAL,
    LABEL_GLOBAL64,
    COMPONENT,
    COMPONENT64,
    SELECT_WINDOW,
    SELECT_WINDOW64,
    SELECT_LASSO,
    SELECT_LASSO64,
    LINE_BUS,
    LINE_BUS64,
    LINE_GRAPHIC,
    LINE_GRAPHIC64,
    LINE_WIRE,
    LINE_WIRE64,
    LABEL_HIER,
    LABEL_HIER64,
    PLACE,
    PLACE64
};

/**
 * Simple class to construct and store cursors against unique ID keys.
 *
 * This can be used to lazily construct cursors as needed for specific
 * applications.
 */
class CURSOR_STORE
{
public:
    /**
     * Definition of a cursor
     */
    struct CURSOR_DEF
    {
        const char** m_xpm;

        ///< The "hotspot" where the cursor "is" in the image
        QPoint m_hotspot;
    };

    /**
     * Get a cursor bundle (wx 3.3+) or appropriate cursor (older versions)
     * @param  aCursorType the cursor type to get
     * @param  aHiDPI whether to prefer HiDPI version for older wx versions
     * @return             the cursor bundle or cursor
     */
    static const QCURSOR_TYPE GetCursor( KICURSOR aCursorType, bool aHiDPI = false );

    /**
     * Get stock cursor type for the given cursor
     * @param  aCursorType the cursor type
     * @return             stock cursor type, or wxCURSOR_MAX if not a stock cursor
     */
    static Qt::CursorShape GetStockCursor( KICURSOR aCursorType );

private:
    /**
     * Construct a store with cursors for all defined types.
     * The store will automatically contain both standard and HiDPI cursors.
     */
    CURSOR_STORE();

#if wxCHECK_VERSION( 3, 3, 0 )
    /**
     * Get a cursor bundle by its ID (wx 3.3+ only)
     * @param  aIdKey the ID key to look up
     * @return        the cursor bundle, if found, else an invalid bundle
     */
    const wxCursorBundle& storeGetBundle( KICURSOR aIdKey ) const;
#else
    /**
     * Get a cursor by its ID, automatically selecting the appropriate resolution
     * @param  aIdKey the ID key to look up
     * @param  aHiDPI whether to prefer HiDPI version if available
     * @return        the cursor, if found, else wxNullCursor
     */
    const QCursor& storeGetCursor( KICURSOR aIdKey, bool aHiDPI = false ) const;
#endif

#if wxCHECK_VERSION( 3, 3, 0 )
    ///< Internal store of cursor bundles for wx 3.3+
    std::map<KICURSOR, wxCursorBundle> m_bundleMap;
#else
    std::map<KICURSOR, QCursor> m_standardCursorMap;
    std::map<KICURSOR, QCursor> m_hidpiCursorMap;
#endif

};

#endif // CURSOR_STORE__H
