#pragma once

#include "data_board_item.hxx"

namespace KIGFX {
class DATA_Circle : public BOARD_ITEM
{
public:
	DATA_Circle() = default;
	DATA_Circle(VECTOR2I, double);

	virtual const BOX2I GetBoundingBox() const override;
	std::string GetClass() const override {
		return "Circle";
	}
	
	VECTOR2D m_centerPoint;
	double m_radius;
};
}

