#include "data_painter.hxx"
#include "data_board_item.hxx"
#include "data_line.hxx"
#include "data_circle.hxx"
#include "data_triangle.hxx"
#include "data_rectangle.hxx"


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
	default:
		break;
	}
}

void KIGFX::DATA_PAINTER::draw(const DATA_Triangle* aTriangle, int aLayer) {
	std::vector<VECTOR2D> drawData;
	drawData.push_back(aTriangle->m_point1);
	drawData.push_back(aTriangle->m_point2);
	drawData.push_back(aTriangle->m_point3);

	m_gal->DrawPolyline(drawData);
}
void KIGFX::DATA_PAINTER::draw(const DATA_Rectangle* a_Rectangle, int aLayer) {
	m_gal->DrawRectangle(a_Rectangle->m_startPoint, a_Rectangle->m_endPoint);
}
void KIGFX::DATA_PAINTER::draw(const DATA_Line* aLine, int aLayer) {
	m_gal->DrawLine(aLine->m_startPoint, aLine->m_endPoint);
}
void KIGFX::DATA_PAINTER::draw(const DATA_Circle* aCircle, int aLayer) {
	m_gal->DrawCircle(aCircle->m_centerPoint, aCircle->m_radius);
}