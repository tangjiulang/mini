#pragma once

#include <vector>

#include "data_board_item.hxx"
#include "shape_line_chain.hxx"
#include "shape_poly_set.hxx"

namespace MINI {
	class DATA_Polyline : public BOARD_ITEM
	{
	public:
		DATA_Polyline(SHAPE_LINE_CHAIN, double = 1);
		DATA_Polyline(SHAPE_LINE_CHAIN, PCB_LAYER_ID aLayer, double = 1);

		virtual const BOX2I GetBoundingBox() const override;

		std::string GetClass() const override {
			return "Polyline";
		}

		SHAPE_LINE_CHAIN m_polyline;

		double m_lineWidth;
	};
}