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

KIGFX::COLOR4D KIGFX::DATA_RENDER_SETTINGS::GetColor(const BOARD_ITEM* aItem, int aLayer) const
{
    COLOR4D color(0, 0, 0, 1);

    if (aItem->Type() == ITEM_TYPE::CIRCLE)
        color = COLOR4D(0.5, 0, 0.5, 0.7);
    else if (aItem->Type() == ITEM_TYPE::LINE)
        color = COLOR4D(0, 0.5, 0.5, 0.7);

    else 
        color = COLOR4D(1, 1, 1, 0.8);

    if (aItem->IsSelected())
        color.a *= 0.8;

    return color;
}