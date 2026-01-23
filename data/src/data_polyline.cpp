#include "DATA_Polyline.hxx"

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/multi_polygon.hpp>

constexpr int intMax = std::numeric_limits<int>::max();
constexpr int intMin = std::numeric_limits<int>::min();

namespace bg = boost::geometry;

typedef bg::model::d2::point_xy<double> Point;
typedef bg::model::polygon<Point> Polygon;
typedef bg::model::multi_polygon<Polygon> MultiPolygon;



KIGFX::DATA_Polyline::DATA_Polyline(SHAPE_LINE_CHAIN polyline, double lineWidth)
	: BOARD_ITEM(ITEM_TYPE::POLYLINE), m_polyline(polyline), m_lineWidth(lineWidth)
{
}

KIGFX::DATA_Polyline::DATA_Polyline(SHAPE_LINE_CHAIN polyline, PCB_LAYER_ID aLayer, double lineWidth)
	: BOARD_ITEM(ITEM_TYPE::POLYLINE, aLayer), m_polyline(polyline), m_lineWidth(lineWidth)
{
}

const BOX2I KIGFX::DATA_Polyline::GetBoundingBox() const
{
	return m_polyline.BBox();
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
