#include "data_thread_painter.hxx"
#include "data_board_item.hxx"
#include "data_line.hxx"
#include "data_circle.hxx"
#include "data_triangle.hxx"
#include "data_rectangle.hxx"
#include "data_polyline.hxx"
#include "data_arc.hxx"
#include "data_polygon.hxx"
#include "data_poly_set.hxx"
#include "data_text.hxx"

MINI::DATA_THREAD_PAINTER::DATA_THREAD_PAINTER(GAL* aGal) :
        PAINTER(aGal)
{
}

bool MINI::DATA_THREAD_PAINTER::Draw(const VIEW_ITEM* aItem, int aLayer)
{
    if(!aItem->IsBOARD_ITEM())
        return false;

    const BOARD_ITEM* item = static_cast<const BOARD_ITEM*>(aItem);

    switch(item->Type())
    {
    case ITEM_TYPE::LINE:
        // draw line
        draw(static_cast<const DATA_Line*>(item), aLayer);
        break;
    case ITEM_TYPE::CIRCLE:
        // draw circle
        draw(static_cast<const DATA_Circle*>(item), aLayer);
        break;
    case ITEM_TYPE::TRIANGLE:
        // darw triangle
        draw(static_cast<const DATA_Triangle*>(item), aLayer);
        break;
    case ITEM_TYPE::RECTANGLE:
        // draw rectangle
        draw(static_cast<const DATA_Rectangle*>(item), aLayer);
        break;
    case ITEM_TYPE::POLYLINE: draw(static_cast<const DATA_Polyline*>(item), aLayer); break;
    case ITEM_TYPE::POLYGON:
        // draw polygon
        draw(static_cast<const DATA_Polygon*>(item), aLayer);
        break;
    case ITEM_TYPE::ARC:
        // draw arc
        draw(static_cast<const DATA_Arc*>(item), aLayer);
        break;
    case ITEM_TYPE::TEXT:
        // draw text
        draw(static_cast<const DATA_Text*>(item), aLayer);
        break;
    default: return false;
    }

    return true;
}

void MINI::DATA_THREAD_PAINTER::draw(const DATA_Triangle* aTriangle, int aLayer)
{
    COLOR4D               color = m_dataSettings.GetColor(aTriangle, aLayer);
    std::vector<VECTOR2D> drawData;
    drawData.push_back(aTriangle->m_point1);
    drawData.push_back(aTriangle->m_point2);
    drawData.push_back(aTriangle->m_point3);
    drawData.push_back(aTriangle->m_point1);

    m_insertVertex->SetStrokeEnabled(true);
    m_insertVertex->SetStrokeColor(color);
    m_insertVertex->SetLineWidth(aTriangle->m_lineWidth);

    m_insertVertex->DrawPolyline(drawData);
}
void MINI::DATA_THREAD_PAINTER::draw(const DATA_Rectangle* a_Rectangle, int aLayer)
{
    COLOR4D color = m_dataSettings.GetColor(a_Rectangle, aLayer);

    m_insertVertex->SetFillEnabled(true);
    m_insertVertex->SetStrokeEnabled(false);
    m_insertVertex->SetFillColor(color);
    m_insertVertex->SetStrokeColor(color);
    m_insertVertex->SetLineWidth(a_Rectangle->m_lineWidth);

    m_insertVertex->DrawRectangle(a_Rectangle->m_rect.GetPosition(),
                                  a_Rectangle->m_rect.GetPosition() + a_Rectangle->m_rect.GetSize());
}

void MINI::DATA_THREAD_PAINTER::draw(const DATA_Line* aLine, int aLayer)
{
    COLOR4D color = m_dataSettings.GetColor(aLine, aLayer);

    m_insertVertex->SetStrokeEnabled(true);
    m_insertVertex->SetStrokeColor(color);
    m_insertVertex->SetLineWidth(aLine->m_lineWidth);

    m_insertVertex->DrawLine(aLine->m_line.GetSeg().A, aLine->m_line.GetSeg().B);
}
void MINI::DATA_THREAD_PAINTER::draw(const DATA_Circle* aCircle, int aLayer)
{
    COLOR4D color = m_dataSettings.GetColor(aCircle, aLayer);

    m_insertVertex->SetFillEnabled(false);
    m_insertVertex->SetStrokeEnabled(true);
    m_insertVertex->SetFillColor(color);
    m_insertVertex->SetStrokeColor(color);

    m_insertVertex->DrawCircle(aCircle->m_circle.GetCenter(), aCircle->m_circle.GetRadius());
}

void MINI::DATA_THREAD_PAINTER::draw(const DATA_Polyline* aPolygon, int aLayer)
{
    COLOR4D color = m_dataSettings.GetColor(aPolygon, aLayer);

    m_insertVertex->SetStrokeEnabled(true);
    m_insertVertex->SetStrokeColor(color);
    m_insertVertex->SetLineWidth(aPolygon->m_lineWidth);

    m_insertVertex->DrawPolyline(aPolygon->GetDrawPolyline(m_view));
}

void MINI::DATA_THREAD_PAINTER::draw(const DATA_Polygon* aPolygon, int aLayer)
{
    COLOR4D color = m_dataSettings.GetColor(aPolygon, aLayer);

    if(const SHAPE_LINE_CHAIN* lodPolyline = aPolygon->GetLodPolyline(m_view))
    {
        m_insertVertex->SetFillEnabled(true);
        m_insertVertex->SetStrokeEnabled(false);
        m_insertVertex->SetFillColor(color);
        m_insertVertex->SetStrokeColor(color);
        m_insertVertex->SetLineWidth(aPolygon->m_lineWidth);
        m_insertVertex->DrawPolygon(*lodPolyline);
        m_insertVertex->SetFillEnabled(false);
        m_insertVertex->SetStrokeEnabled(true);
        return;
    }

    for(auto segment : aPolygon->m_segments)
    {
        if(std::holds_alternative<SHAPE_SEGMENT>(segment))
        {
            auto& shape_line = std::get<SHAPE_SEGMENT>(segment);

            m_insertVertex->SetFillEnabled(false);
            m_insertVertex->SetStrokeEnabled(true);
            m_insertVertex->SetFillColor(color);
            m_insertVertex->SetStrokeColor(color);
            m_insertVertex->SetLineWidth(aPolygon->m_lineWidth);

            m_insertVertex->DrawSegment(shape_line.GetSeg().A, shape_line.GetSeg().B);
        }
        else
        {
            auto&     shape_arc = std::get<SHAPE_ARC>(segment);
            EDA_ANGLE startAngle, endAngle;
            startAngle = shape_arc.GetStartAngle();
            endAngle = shape_arc.GetEndAngle();

            m_insertVertex->SetFillEnabled(false);
            m_insertVertex->SetStrokeEnabled(true);
            m_insertVertex->SetFillColor(color);
            m_insertVertex->SetStrokeColor(color);
            m_insertVertex->SetLineWidth(aPolygon->m_lineWidth);

            m_insertVertex->DrawArcSegment(shape_arc.GetCenter(), shape_arc.GetRadius(), startAngle,
                                           (endAngle - startAngle).Normalize(), aPolygon->m_lineWidth, 0);
        }
    }
}

void MINI::DATA_THREAD_PAINTER::draw(const DATA_Arc* aArc, int aLayer)
{
    COLOR4D   color = m_dataSettings.GetColor(aArc, aLayer);
    EDA_ANGLE startAngle, endAngle;
    startAngle = aArc->m_arc.GetStartAngle();
    endAngle = aArc->m_arc.GetEndAngle();

    m_insertVertex->SetFillEnabled(false);
    m_insertVertex->SetStrokeEnabled(true);
    m_insertVertex->SetFillColor(color);
    m_insertVertex->SetStrokeColor(color);
    m_insertVertex->SetLineWidth(aArc->m_lineWidth);

    m_insertVertex->DrawArc(aArc->m_arc.GetCenter(), aArc->m_arc.GetRadius(), startAngle,
                            (endAngle - startAngle).Normalize());
}

void MINI::DATA_THREAD_PAINTER::draw(DATA_PolySet* aPolySet, int aLayer)
{
    COLOR4D color = m_dataSettings.GetColor(aPolySet, aLayer);

    const SHAPE_POLY_SET& drawPolySet = aPolySet->GetDrawPolySet(m_view);

    if(!drawPolySet.IsTriangulationUpToDate())
        const_cast<SHAPE_POLY_SET&>(drawPolySet).CacheTriangulation(true, true);

    m_insertVertex->SetFillEnabled(true);
    m_insertVertex->SetStrokeEnabled(false);
    m_insertVertex->SetFillColor(color);
    m_insertVertex->SetStrokeColor(color);
    m_insertVertex->SetLineWidth(aPolySet->m_lineWidth);

    m_insertVertex->DrawPolygon(drawPolySet, true);
}

void MINI::DATA_THREAD_PAINTER::draw(const DATA_Text* aText, int aLayer)
{
    const KIFONT::METRICS& metrics = KIFONT::METRICS::Default();
    TEXT_ATTRIBUTES        attrs = aText->m_textAttrs;
    const COLOR4D&         color = m_dataSettings.GetColor(aText, aLayer);

    m_insertVertex->SetStrokeColor(color);
    m_insertVertex->SetFillColor(color);

    attrs.m_StrokeWidth = aText->GetEffectiveTextPenWidth();

    strokeText(aText->m_text, aText->m_position, attrs, metrics);
}

void MINI::DATA_THREAD_PAINTER::strokeText(const std::string& aText, const VECTOR2I& aPosition,
                                           const TEXT_ATTRIBUTES& aAttrs, const KIFONT::METRICS& aFontMetrics)
{
    KIFONT::FONT* font = aAttrs.m_Font;

    if(!font)
        font = KIFONT::FONT::GetFont("", aAttrs.m_Bold, aAttrs.m_Italic);

    m_insertVertex->SetFillEnabled(font->IsOutline());
    m_insertVertex->SetStrokeEnabled(font->IsStroke());

    VECTOR2I pos(aPosition);
    VECTOR2I fudge(KiROUND(0.16 * aAttrs.m_StrokeWidth), 0);

    if((aAttrs.m_Halign == GR_TEXT_H_ALIGN_LEFT && !aAttrs.m_Mirrored)
       || (aAttrs.m_Halign == GR_TEXT_H_ALIGN_RIGHT && aAttrs.m_Mirrored))
    {
        pos -= fudge;
    }
    else if((aAttrs.m_Halign == GR_TEXT_H_ALIGN_RIGHT && !aAttrs.m_Mirrored)
            || (aAttrs.m_Halign == GR_TEXT_H_ALIGN_LEFT && aAttrs.m_Mirrored))
    {
        pos += fudge;
    }

    font->Draw(m_insertVertex, aText, pos, aAttrs, aFontMetrics);
}
