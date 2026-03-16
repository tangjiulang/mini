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

MINI::DATA_THREAD_PAINTER::DATA_THREAD_PAINTER(GAL* aGal)
	: PAINTER(aGal) {
}

bool MINI::DATA_THREAD_PAINTER::Draw(const VIEW_ITEM* aItem, int aLayer) {
	if (!aItem->IsBOARD_ITEM())
		return false;

	const BOARD_ITEM* item = static_cast<const BOARD_ITEM*>(aItem);

	switch (item->Type())
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
	case ITEM_TYPE::POLYLINE:
		draw(static_cast<const DATA_Polyline*>(item), aLayer);
		break;
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
        //draw(static_cast<const DATA_Text*>(item), aLayer);
        break;
	default:
		break;
	}
}

void MINI::DATA_THREAD_PAINTER::draw(const DATA_Triangle* aTriangle, int aLayer) {
	COLOR4D color = m_dataSettings.GetColor(aTriangle, aLayer);
	std::vector<VECTOR2D> drawData;
	drawData.push_back(aTriangle->m_point1);
	drawData.push_back(aTriangle->m_point2);
	drawData.push_back(aTriangle->m_point3);
	drawData.push_back(aTriangle->m_point1);

	m_insertVertex->DrawPolyline(drawData, color, aTriangle->m_lineWidth);
}
void MINI::DATA_THREAD_PAINTER::draw(const DATA_Rectangle* a_Rectangle, int aLayer) {
	COLOR4D color = m_dataSettings.GetColor(a_Rectangle, aLayer);
	m_insertVertex->DrawRectangle(a_Rectangle->m_rect.GetPosition(), a_Rectangle->m_rect.GetPosition() + a_Rectangle->m_rect.GetSize(), false, true, color, color, a_Rectangle->m_lineWidth);
}
void MINI::DATA_THREAD_PAINTER::draw(const DATA_Line* aLine, int aLayer) {
	COLOR4D color = m_dataSettings.GetColor(aLine, aLayer);
	m_insertVertex->DrawLine(aLine->m_line.GetSeg().A, aLine->m_line.GetSeg().B, color, aLine->m_lineWidth);
}
void MINI::DATA_THREAD_PAINTER::draw(const DATA_Circle* aCircle, int aLayer) {
	COLOR4D color = m_dataSettings.GetColor(aCircle, aLayer);
	m_insertVertex->DrawCircle(aCircle->m_circle.GetCenter(), aCircle->m_circle.GetRadius(), false, true, color, color);
}

void MINI::DATA_THREAD_PAINTER::draw(const DATA_Polyline* aPolygon, int aLayer)
{
	COLOR4D color = m_dataSettings.GetColor(aPolygon, aLayer);
	m_insertVertex->DrawPolyline(aPolygon->m_polyline, color, aPolygon->m_lineWidth);
}

void MINI::DATA_THREAD_PAINTER::draw(const DATA_Polygon* aPolygon, int aLayer) {
	COLOR4D color = m_dataSettings.GetColor(aPolygon, aLayer);
	for (auto segment : aPolygon->m_segments) {
		if (std::holds_alternative<SHAPE_SEGMENT>(segment)) {
			auto& shape_line = std::get<SHAPE_SEGMENT>(segment);
			m_insertVertex->DrawSegment(shape_line.GetSeg().A, shape_line.GetSeg().B, false, true, color, color, aPolygon->m_lineWidth);
		}
		else {
			auto& shape_arc = std::get<SHAPE_ARC>(segment);
			EDA_ANGLE startAngle, endAngle;
			startAngle = shape_arc.GetStartAngle();
			endAngle = shape_arc.GetEndAngle();
			m_insertVertex->DrawArcSegment(shape_arc.GetCenter(), shape_arc.GetRadius(), startAngle, (endAngle - startAngle).Normalize(), false, true, color, color, aPolygon->m_lineWidth, aPolygon->m_lineWidth, 0);
		}
	}
}

void MINI::DATA_THREAD_PAINTER::draw(const DATA_Arc* aArc, int aLayer)
{
	COLOR4D color = m_dataSettings.GetColor(aArc, aLayer);
	EDA_ANGLE startAngle, endAngle;
	startAngle = aArc->m_arc.GetStartAngle();
	endAngle = aArc->m_arc.GetEndAngle();
	m_insertVertex->DrawArc(aArc->m_arc.GetCenter(), aArc->m_arc.GetRadius(), startAngle, (endAngle - startAngle).Normalize(), false, true, color, color, aArc->m_lineWidth);
}

void MINI::DATA_THREAD_PAINTER::draw(DATA_PolySet* aPolySet, int aLayer)
{
	COLOR4D color = m_dataSettings.GetColor(aPolySet, aLayer);
	m_gal->SetStrokeColor(color);
	m_gal->SetFillColor(color);
	m_gal->SetLineWidth(0);
	m_gal->SetIsFill(true);
	m_gal->SetIsStroke(false);

	if (!aPolySet->m_polySet.IsTriangulationUpToDate())
		aPolySet->m_polySet.CacheTriangulation(true, true);

	m_insertVertex->DrawPolygon(aPolySet->m_polySet, true, true, false, color, color, aPolySet->m_lineWidth);
}