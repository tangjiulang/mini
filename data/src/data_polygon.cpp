#include "data_polygon.hxx"

MINI::DATA_Polygon::DATA_Polygon(std::vector<Segment> points, double lineWidth)
	: BOARD_ITEM(ITEM_TYPE::POLYGON),
	  m_segments(std::move(points)),
	  m_lineWidth(lineWidth) {}

MINI::DATA_Polygon::DATA_Polygon(std::vector<Segment> points, PCB_LAYER_ID aLayer, double lineWidth)
	: BOARD_ITEM(ITEM_TYPE::POLYGON, aLayer),
	  m_segments(std::move(points)),
	  m_lineWidth(lineWidth) {}

const BOX2I MINI::DATA_Polygon::GetBoundingBox() const
{
	BOX2I box;
	for (auto Segment : m_segments) {
		if (std::holds_alternative<SHAPE_SEGMENT>(Segment)) {
			auto segment = std::get<SHAPE_SEGMENT>(Segment);
			box.Merge(segment.BBox());
		}
		else if (std::holds_alternative<SHAPE_ARC>(Segment)) {
			auto arc = std::get<SHAPE_ARC>(Segment);
			box.Merge(arc.BBox());
		}
	}

	return box;
}