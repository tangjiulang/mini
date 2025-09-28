#include <gal/include/gal_display_options.hxx>

#include <spdlog/spdlog.h>
#include <dpi_scaling.hxx>

using namespace KIGFX;


/**
 * Flag to enable GAL_DISPLAY_OPTIONS logging
 *
 * Use "KICAD_GAL_DISPLAY_OPTIONS" to enable.
 *
 * @ingroup trace_env_vars
 */
static const char* traceGalDispOpts = "KICAD_GAL_DISPLAY_OPTIONS";


GAL_DISPLAY_OPTIONS::GAL_DISPLAY_OPTIONS()
    : antialiasing_mode( GAL_ANTIALIASING_MODE::AA_NONE ),
      m_gridStyle( GRID_STYLE::DOTS ),
      m_gridSnapping( GRID_SNAPPING::ALWAYS ),
      m_gridLineWidth( 1.0 ),
      m_gridMinSpacing( 10.0 ),
      m_axesEnabled( false ),
      m_fullscreenCursor( false ),
      m_forceDisplayCursor( false ),
      m_scaleFactor( DPI_SCALING::GetDefaultScaleFactor() )
{
}


void GAL_DISPLAY_OPTIONS::NotifyChanged()
{
    spdlog::trace( "[{}] Change notification", traceGalDispOpts);

    Notify( &GAL_DISPLAY_OPTIONS_OBSERVER::OnGalDisplayOptionsChanged, *this );
}