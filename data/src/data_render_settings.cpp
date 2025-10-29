#include "data_render_settings.hxx"

using namespace KIGFX;

DATA_RENDER_SETTINGS::DATA_RENDER_SETTINGS() {
    m_backgroundColor = COLOR4D(0.0, 0.0, 0.0, 1.0);
    //m_ZoneDisplayMode = ZONE_DISPLAY_MODE::SHOW_FILLED;
    //m_netColorMode = NET_COLOR_MODE::RATSNEST;
    //m_ContrastModeDisplay = HIGH_CONTRAST_MODE::NORMAL;

    m_trackOpacity = 1.0;
    m_viaOpacity = 1.0;
    m_padOpacity = 1.0;
    m_zoneOpacity = 1.0;
    m_imageOpacity = 1.0;
    m_filledShapeOpacity = 1.0;

    m_ForcePadSketchModeOn = false;

    //m_PadEditModePad = nullptr;

    SetDashLengthRatio(12);       // From ISO 128-2
    SetGapLengthRatio(3);         // From ISO 128-2

    m_ForceShowFieldsWhenFPSelected = true;

    update();
}

void DATA_RENDER_SETTINGS::LoadColors(const COLOR_SETTINGS* aSettings) {}

bool DATA_RENDER_SETTINGS::GetShowPageLimits() const { return false; }