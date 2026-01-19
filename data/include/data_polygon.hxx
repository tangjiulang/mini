#pragma once

#include <vector>

#include "data_board_item.hxx"

namespace KIGFX {
	class DATA_Polygon : public BOARD_ITEM
	{
	public:
		DATA_Polygon(std::vector<VECTOR2I> points, double = 1);

		virtual const BOX2I GetBoundingBox() const override;

		bool IsSelfInterSection();
		bool ShouldBeCorrect();

		bool CorrectPolygon();
		std::string GetClass() const override {
			return "Polygon";
		}
		std::vector<VECTOR2I> m_points;

		double m_lineWidth;
	};
}