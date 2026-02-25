#pragma once

#include "data_board_item.hxx"
#include "shape_circle.hxx"

namespace MINI {
class DATA_Circle : public BOARD_ITEM
{
public:
	DATA_Circle() = default;
	DATA_Circle(SHAPE_CIRCLE circle, double = 1);
	DATA_Circle(SHAPE_CIRCLE circle, PCB_LAYER_ID aLayer, double = 1);

	virtual const BOX2I GetBoundingBox() const override;
	std::string GetClass() const override {
		return "Circle";
	}
	
	SHAPE_CIRCLE m_circle;

	double m_lineWidth;
};
}

