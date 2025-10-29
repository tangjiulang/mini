#include "data_triangle.hxx"

using namespace KIGFX;

DATA_Triangle::DATA_Triangle(VECTOR2I aPoint1, VECTOR2I aPoint2, VECTOR2I aPoint3)
	: BOARD_ITEM(ITEM_TYPE::TRIANGLE),
	  m_point1(aPoint1),
	  m_point2(aPoint2),
	  m_point3(aPoint3) {}

const BOX2I DATA_Triangle::GetBoundingBox() const
{
	VECTOR2I startPoint = { std::min({m_point1.x, m_point2.x, m_point3.x}), std::min({m_point1.x, m_point2.x, m_point3.x}) };
	VECTOR2I endPoint = { std::max({m_point1.x, m_point2.x, m_point3.x}), std::max({m_point1.x, m_point2.x, m_point3.x}) };
	VECTOR2I dis = { endPoint.x - startPoint.x, endPoint.y - startPoint.y };
	return BOX2I(startPoint, dis);
}