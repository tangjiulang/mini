#pragma once

#include "data_board_item.hxx"
#include "shape_segment.hxx"

namespace MINI {
class DATA_Line : public BOARD_ITEM
{
public:
	DATA_Line(SHAPE_SEGMENT, double = 1);
	DATA_Line(SHAPE_SEGMENT, PCB_LAYER_ID aLayer, double = 1);

	virtual const BOX2I GetBoundingBox() const override;
	std::string GetClass() const override {
		return "Line";
	}

	SHAPE_SEGMENT m_line;

	double m_lineWidth;
};
}

