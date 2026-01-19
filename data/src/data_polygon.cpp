#include "data_polygon.hxx"

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/multi_polygon.hpp>

constexpr int intMax = std::numeric_limits<int>::max();
constexpr int intMin = std::numeric_limits<int>::min();

namespace bg = boost::geometry;

typedef bg::model::d2::point_xy<double> Point;
typedef bg::model::polygon<Point> Polygon;
typedef bg::model::multi_polygon<Polygon> MultiPolygon;



KIGFX::DATA_Polygon::DATA_Polygon(std::vector<VECTOR2I> points, double lineWidth)
	: BOARD_ITEM(KIGFX::POLYGON), m_points(points), m_lineWidth(lineWidth)
{
}

const BOX2I KIGFX::DATA_Polygon::GetBoundingBox() const
{
	VECTOR2I leftTop = { intMax, intMax }, rightButtom = { intMin, intMin };
	for (auto point : m_points) {
		leftTop.x = std::min(leftTop.x, point.x);
		leftTop.y = std::min(leftTop.y, point.y);
		rightButtom.x = std::max(rightButtom.x, point.x);
		rightButtom.y = std::max(rightButtom.y, point.y);
	}
	return BOX2I(leftTop, VECTOR2I(rightButtom.x - leftTop.x, rightButtom.y - leftTop.y));
}


bool CorrectByBoost(std::vector<VECTOR2I>& points) {
	Polygon input;
	for (auto point : points)
		input.outer().push_back(Point(point.x, point.y));

	MultiPolygon result;
	bg::union_(input, input, result);


	if (result.empty()) {
		points.clear();
		for (auto bgPoint : input.outer())
			points.push_back(VECTOR2I(bgPoint.x(), bgPoint.y()));
		return true;
	}
	else {
		int index = 0;
		double area = intMin;
		for (int i = 0; i < result.size(); i++) {
			auto& polygon = result[i];
			auto bgArea = bg::area(polygon);
			if (area < bgArea) {
				index = i;
				area = bgArea;
			}
		}
		points.clear();
		for (auto bgPoint : result[index].outer())
			points.push_back(VECTOR2I(bgPoint.x(), bgPoint.y()));

		return true;
	}
	return false;
}


bool KIGFX::DATA_Polygon::CorrectPolygon()
{
	CorrectByBoost(m_points);
	return true;
}