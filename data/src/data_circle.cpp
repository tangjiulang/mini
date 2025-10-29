#include "data_circle.hxx"

using namespace KIGFX;

DATA_Circle::DATA_Circle(VECTOR2I aCenterPoint, double aRadius)
	: BOARD_ITEM(ITEM_TYPE::CIRCLE),
	  m_centerPoint(aCenterPoint),
	  m_radius(aRadius) { }

const BOX2I DATA_Circle::GetBoundingBox() const
{
	VECTOR2I pos = m_centerPoint - VECTOR2I(m_radius / 2, m_radius / 2);
	return BOX2I(pos, VECTOR2I(m_radius, m_radius));
}

