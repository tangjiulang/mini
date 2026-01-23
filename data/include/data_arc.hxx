#pragma once
#include <vector>
#include "vector2d.hxx"
#include "data_board_item.hxx"
#include "shape_arc.hxx"

namespace KIGFX {
	class DATA_Arc : public BOARD_ITEM
	{
	public:
		DATA_Arc(SHAPE_ARC arc, double = 1);
		DATA_Arc(SHAPE_ARC arc, PCB_LAYER_ID aLayer, double = 1);

		virtual const BOX2I GetBoundingBox() const override;

		std::string GetClass() const override {
			return "Polygon";
		}
		SHAPE_ARC m_arc;

		double m_lineWidth;
	};
}