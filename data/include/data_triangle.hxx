#pragma once

#include "data_board_item.hxx"

namespace KIGFX {
class DATA_Triangle : public BOARD_ITEM
{
public:
	DATA_Triangle(VECTOR2I, VECTOR2I, VECTOR2I);

	virtual const BOX2I GetBoundingBox() const override;
	std::string GetClass() const override {
		return "Triangle";
	}

	VECTOR2I m_point1;
	VECTOR2I m_point2;
	VECTOR2I m_point3;
};
}

