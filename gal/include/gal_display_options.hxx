#ifndef GAL_DISPLAY_OPTIONS_H__
#define GAL_DISPLAY_OPTIONS_H__

#include "gal/include/gal.hxx"
#include "dpi_scaling.hxx"
#include "observable.hxx"

class COMMON_SETTINGS;
struct WINDOW_SETTINGS;


#if defined( _MSC_VER )
#pragma warning( push )
#pragma warning( disable : 4275 )
#endif

namespace KIGFX
{
    /**
     * Type definition of the grid style.
     */
    enum class GRID_STYLE
    {
        DOTS,       ///< Use dots for the grid
        LINES,      ///< Use lines for the grid
        SMALL_CROSS ///< Use small cross instead of dots for the grid
    };

    enum class GAL_ANTIALIASING_MODE
    {
        AA_NONE,
        AA_FAST,
        AA_HIGHQUALITY,
    };

    enum class GRID_SNAPPING
    {
        ALWAYS,
        WITH_GRID,
        NEVER
    };

    class GAL_DISPLAY_OPTIONS;

    class GAL_DISPLAY_OPTIONS_OBSERVER
    {
    public:
        virtual void OnGalDisplayOptionsChanged( const GAL_DISPLAY_OPTIONS& ) = 0;

    protected:
        // Observer lifetimes aren't handled by base class pointer
        virtual ~GAL_DISPLAY_OPTIONS_OBSERVER() {}
    };

    class GAL_DISPLAY_OPTIONS : public UTIL::OBSERVABLE<GAL_DISPLAY_OPTIONS_OBSERVER>
    {
    public:
        GAL_DISPLAY_OPTIONS();

        virtual ~GAL_DISPLAY_OPTIONS()
        {}

        GAL_ANTIALIASING_MODE antialiasing_mode;

        ///< The grid style to draw the grid in
        KIGFX::GRID_STYLE m_gridStyle;

        ///< Snapping options for the grid
        GRID_SNAPPING m_gridSnapping;

        ///< Thickness to render grid lines/dots
        double m_gridLineWidth;

        ///< Minimum pixel distance between displayed grid lines
        double m_gridMinSpacing;

        ///< Whether or not to draw the coordinate system axes
        bool m_axesEnabled;

        ///< Fullscreen crosshair or small cross
        bool m_fullscreenCursor;

        ///< Force cursor display
        bool m_forceDisplayCursor;

        ///< The pixel scale factor (>1 for hi-DPI scaled displays)
        double m_scaleFactor;

        void NotifyChanged();
    };

} // namespace KIGFX

#if defined( _MSC_VER )
#pragma warning( pop )
#endif

#endif

