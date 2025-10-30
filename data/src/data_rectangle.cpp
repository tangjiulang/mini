#include "data_rectangle.hxx"

using namespace KIGFX;

DATA_Rectangle::DATA_Rectangle(VECTOR2I aStartPoint, VECTOR2I aEndPoint)
	: BOARD_ITEM(ITEM_TYPE::RECTANGLE),
	  m_startPoint(aStartPoint),
	  m_endPoint(aEndPoint) { }

const BOX2I DATA_Rectangle::GetBoundingBox() const
{
	double dx = abs(m_endPoint.x - m_startPoint.x);
	double dy = abs(m_endPoint.y - m_startPoint.y);
	VECTOR2D pos = { std::min(m_startPoint.x, m_endPoint.x), std::min(m_startPoint.y, m_endPoint.y) };
	VECTOR2D dis = { dx, dy };
	return BOX2I(pos, dis);
}
