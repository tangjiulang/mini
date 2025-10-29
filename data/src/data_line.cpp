#include "data_line.hxx"

using namespace KIGFX;

DATA_Line::DATA_Line(VECTOR2I aStartPoint, VECTOR2I aEndPoint) 
	: BOARD_ITEM(ITEM_TYPE::LINE),
	  m_startPoint(aStartPoint),
	  m_endPoint(aEndPoint) {}

const BOX2I DATA_Line::GetBoundingBox() const
{
	int dx = abs(m_endPoint.x - m_startPoint.x);
	int dy = abs(m_endPoint.y - m_startPoint.y);
	VECTOR2I pos = { std::min(m_startPoint.x, m_endPoint.x), std::min(m_startPoint.y, m_endPoint.y) };
	VECTOR2I dis = { dx, dy };
	return BOX2I(pos, dis);
}
