#include "data_painter.hxx"
#include "data_board_item.hxx"
#include "data_line.hxx"
#include "data_circle.hxx"
#include "data_triangle.hxx"
#include "data_rectangle.hxx"
#include "data_polygon.hxx"

KIGFX::DATA_PAINTER::DATA_PAINTER(GAL* aGal)
	: PAINTER(aGal) { }

bool KIGFX::DATA_PAINTER::Draw(const VIEW_ITEM* aItem, int aLayer) {
	if (!aItem->IsBOARD_ITEM())
		return false;

	const BOARD_ITEM* item = static_cast<const BOARD_ITEM*>(aItem);

	switch (item->Type())
	{
	case ITEM_TYPE::LINE :
		// draw line
		draw(static_cast<const DATA_Line*>(item), aLayer);
		break;
	case ITEM_TYPE::CIRCLE :
		// draw circle
		draw(static_cast<const DATA_Circle*>(item), aLayer);
		break;
	case ITEM_TYPE::TRIANGLE :
		// darw triangle
		draw(static_cast<const DATA_Triangle*>(item), aLayer);
		break;
	case ITEM_TYPE::RECTANGLE :
		// draw rectangle
		draw(static_cast<const DATA_Rectangle*>(item), aLayer);
		break;
	case ITEM_TYPE::POLYGON :
		draw(static_cast<const DATA_Polygon*>(item), aLayer);
		break;
	default:
		break;
	}
}

void KIGFX::DATA_PAINTER::draw(const DATA_Triangle* aTriangle, int aLayer) {
	std::vector<VECTOR2D> drawData;
	drawData.push_back(aTriangle->m_point1);
	drawData.push_back(aTriangle->m_point2);
	drawData.push_back(aTriangle->m_point3);
	drawData.push_back(aTriangle->m_point1);

	m_gal->SetLineWidth(aTriangle->m_lineWidth);

	m_gal->DrawPolyline(drawData);
}
void KIGFX::DATA_PAINTER::draw(const DATA_Rectangle* a_Rectangle, int aLayer) {
	COLOR4D color = m_dataSettings.GetColor(a_Rectangle, aLayer);
	m_gal->SetStrokeColor(color);
	m_gal->SetFillColor(color);

	m_gal->SetLineWidth(a_Rectangle->m_lineWidth);

	m_gal->DrawRectangle(a_Rectangle->m_startPoint, a_Rectangle->m_endPoint);
}
void KIGFX::DATA_PAINTER::draw(const DATA_Line* aLine, int aLayer) {
	COLOR4D color = m_dataSettings.GetColor(aLine, aLayer);
	m_gal->SetStrokeColor(color);
	m_gal->SetFillColor(color);

	m_gal->SetLineWidth(aLine->m_lineWidth);

	m_gal->DrawLine(aLine->m_startPoint, aLine->m_endPoint);
}
void KIGFX::DATA_PAINTER::draw(const DATA_Circle* aCircle, int aLayer) {
	COLOR4D color = m_dataSettings.GetColor(aCircle, aLayer);
	m_gal->SetStrokeColor(color);
	m_gal->SetFillColor(color);

	m_gal->SetLineWidth(aCircle->m_lineWidth);

	m_gal->DrawCircle(aCircle->m_centerPoint, aCircle->m_radius);
}

void KIGFX::DATA_PAINTER::draw(const DATA_Polygon* aPolygon, int aLayer)
{
	std::deque<VECTOR2D> points;
	for (auto point : aPolygon->m_points) points.push_back(point);
	COLOR4D color = m_dataSettings.GetColor(aPolygon, aLayer);
	m_gal->SetStrokeColor(color);
	m_gal->SetFillColor(color);
	
	m_gal->SetLineWidth(aPolygon->m_lineWidth);

	m_gal->DrawPolygon(points);
}