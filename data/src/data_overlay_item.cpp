#include <data_overlay_item.hxx>

#include <gal/include/graphics_abstraction_layer.hxx>
#include <view.hxx>

using namespace KIGFX;


SIMPLE_OVERLAY_ITEM::SIMPLE_OVERLAY_ITEM() :
    BOARD_ITEM(NOT_USED),    // this item is never added to a BOARD so it needs no type.
    m_fillColor(WHITE),
    m_strokeColor(WHITE),
    m_lineWidth(1.0)
{
}


void SIMPLE_OVERLAY_ITEM::ViewDraw(int aLayer, KIGFX::VIEW* aView) const
{
    KIGFX::GAL& gal = *aView->GetGAL();

    setupGal(gal);
    drawPreviewShape(aView);
}


std::vector<int> SIMPLE_OVERLAY_ITEM::ViewGetLayers() const
{
    std::vector<int> aLayers{ LAYER_GP_OVERLAY };
    return aLayers;
}


void SIMPLE_OVERLAY_ITEM::setupGal(KIGFX::GAL& aGal) const
{
    // default impl: set up the GAL options we have - the
    // overriding class can add to this if needed
    aGal.SetLineWidth(m_lineWidth);
    aGal.SetStrokeColor(m_strokeColor);
    aGal.SetFillColor(m_fillColor);
    aGal.SetIsStroke(true);
    aGal.SetIsFill(true);
}
