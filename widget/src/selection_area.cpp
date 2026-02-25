#include <selection_area.hxx>

#include <gal/include/graphics_abstraction_layer.hxx>
#include <gal/include/painter.hxx>
#include <view.hxx>

using namespace MINI;
using MINI::COLOR4D;

struct SELECTION_COLORS
{
    COLOR4D normal;
    COLOR4D additive;
    COLOR4D subtract;
    COLOR4D exclusiveOr;
    COLOR4D outline_l2r;
    COLOR4D outline_r2l;
};

static const SELECTION_COLORS selectionColorScheme[2] = {
    { // dark background
        COLOR4D(0.3, 0.3, 0.7, 0.3), // Slight blue
        COLOR4D(0.3, 0.7, 0.3, 0.3), // Slight green
        COLOR4D(0.7, 0.3, 0.3, 0.3), // Slight red
        COLOR4D(0.7, 0.3, 0.3, 0.3), // Slight red

        COLOR4D(1.0, 1.0, 0.4, 1.0), // yellow
        COLOR4D(0.4, 0.4, 1.0, 1.0) // blue
    },
    { // bright background
        COLOR4D(0.5, 0.3, 1.0, 0.5), // Slight blue
        COLOR4D(0.5, 1.0, 0.5, 0.5), // Slight green
        COLOR4D(1.0, 0.5, 0.5, 0.5), // Slight red
        COLOR4D(1.0, 0.5, 0.5, 0.5), // Slight red

        COLOR4D(0.7, 0.7, 0.0, 1.0), // yellow
        COLOR4D(0.1, 0.1, 1.0, 1.0) // blue
    }
};


SELECTION_AREA::SELECTION_AREA() :
    m_additive(false),
    m_subtractive(false),
    m_exclusiveOr(false),
    m_mode(SELECTION_MODE::INSIDE_RECTANGLE)
{

}


const BOX2I SELECTION_AREA::ViewBBox() const
{
    BOX2I tmp;

    switch (m_mode)
    {
    default:
    case SELECTION_MODE::INSIDE_RECTANGLE:
    case SELECTION_MODE::TOUCHING_RECTANGLE:
        tmp.SetOrigin(m_origin);
        tmp.SetEnd(m_end);
        break;
    case SELECTION_MODE::INSIDE_LASSO:
    case SELECTION_MODE::TOUCHING_LASSO:
        tmp = m_shape_poly.BBox();
        break;
    }

    tmp.Normalize();

    return tmp;
}


void SELECTION_AREA::ViewDraw(int aLayer, MINI::VIEW* aView) const
{
    MINI::GAL& gal = *aView->GetGAL();
    RENDER_SETTINGS* settings = aView->GetPainter()->GetSettings();

    const SELECTION_COLORS& scheme = settings->IsBackgroundDark() ? selectionColorScheme[0]
        : selectionColorScheme[1];

    // Set the colors of the selection shape based on the selection mode
    if (m_additive)
        gal.SetFillColor(scheme.additive);
    else if (m_subtractive)
        gal.SetFillColor(scheme.subtract);
    else if (m_exclusiveOr)
        gal.SetFillColor(scheme.exclusiveOr);
    else
        gal.SetFillColor(scheme.normal);

    if (m_mode == SELECTION_MODE::INSIDE_RECTANGLE || m_mode == SELECTION_MODE::INSIDE_LASSO)
        gal.SetStrokeColor(scheme.outline_l2r);
    else
        gal.SetStrokeColor(scheme.outline_r2l);

    auto drawSelectionShape =
        [&]()
        {
            switch (m_mode)
            {
            default:
            case SELECTION_MODE::INSIDE_RECTANGLE:
            case SELECTION_MODE::TOUCHING_RECTANGLE:
                gal.DrawRectangle(m_origin, m_end);
                break;
            case SELECTION_MODE::INSIDE_LASSO:
            case SELECTION_MODE::TOUCHING_LASSO:
                if (m_shape_poly.PointCount() > 1)
                    gal.DrawPolygon(m_shape_poly);
                break;
            }
        };

    gal.SetIsStroke(true);
    gal.SetIsFill(false);
    // force 1-pixel-wide line
    gal.SetLineWidth(0.0);
    drawSelectionShape();

    // draw the fill as the second object so that Z test will not clamp
    // the single-pixel-wide rectangle sides
    gal.SetIsFill(true);
    drawSelectionShape();
}

const BOX2I MINI::SELECTION_AREA::GetBoundingBox() const
{
    return BOX2I(m_origin, m_end - m_origin);
}
