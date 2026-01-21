#pragma once

#include "data_board_item.hxx"
#include "shape_rect.hxx"

namespace KIGFX {
class DATA_Rectangle : public BOARD_ITEM
{
public:
	DATA_Rectangle() = default;
	DATA_Rectangle(SHAPE_RECT, double = 1);

	virtual const BOX2I GetBoundingBox() const override;
	std::string GetClass() const override {
		return "Rectangle";
	}

	SHAPE_RECT m_rect;

	double m_lineWidth;
};
}

