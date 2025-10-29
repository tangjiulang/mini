#include "data_rectangle.hxx"

using namespace KIGFX;

DATA_Rectangle::DATA_Rectangle(VECTOR2I aStartPoint, VECTOR2I aEndPoint)
	: BOARD_ITEM(ITEM_TYPE::RECTANGLE),
	  m_startPoint(aStartPoint),
	  m_endPoint(aEndPoint) { }

const BOX2I DATA_Rectangle::GetBoundingBox() const
{
	int dx = abs(m_endPoint.x - m_startPoint.x);
	int dy = abs(m_endPoint.y - m_startPoint.y);
	VECTOR2I pos = { std::min(m_startPoint.x, m_endPoint.x), std::min(m_startPoint.y, m_endPoint.y) };
	VECTOR2I dis = { dx, dy };
	return BOX2I(pos, dis);
}
