#include "data_circle.hxx"

using namespace KIGFX;

DATA_Circle::DATA_Circle(VECTOR2I aCenterPoint, double aRadius, double lineWidth)
	: BOARD_ITEM(ITEM_TYPE::CIRCLE),
	  m_centerPoint(aCenterPoint),
	  m_radius(aRadius),
	  m_lineWidth(lineWidth) {}

const BOX2I DATA_Circle::GetBoundingBox() const
{
	VECTOR2I pos = m_centerPoint - VECTOR2I(m_radius, m_radius);
	return BOX2I(pos, VECTOR2I(2 * m_radius, 2 * m_radius));
}

