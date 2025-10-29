#pragma once

#include "data_board_item.hxx"

namespace KIGFX {
class DATA_Rectangle : public BOARD_ITEM
{
public:
	DATA_Rectangle(VECTOR2I, VECTOR2I);

	virtual const BOX2I GetBoundingBox() const override;
	std::string GetClass() const override {
		return "Rectangle";
	}

	VECTOR2I m_startPoint;
	VECTOR2I m_endPoint;
};
}

