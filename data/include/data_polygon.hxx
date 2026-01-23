#pragma once
#include <vector>
#include "vector2d.hxx"
#include "data_board_item.hxx"
#include "shape_arc.hxx"
#include "shape_segment.hxx"



namespace KIGFX {

	using Segment = std::variant<SHAPE_ARC, SHAPE_SEGMENT>;

	class DATA_Polygon : public BOARD_ITEM
	{
	public:
		DATA_Polygon(std::vector<Segment> points, double = 1);
		DATA_Polygon(std::vector<Segment> points, PCB_LAYER_ID aLayer, double = 1);

		virtual const BOX2I GetBoundingBox() const override;

		std::string GetClass() const override {
			return "Polygon";
		}

		std::vector<Segment> m_segments;

		double m_lineWidth;
	};
}