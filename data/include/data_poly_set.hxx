#pragma once

#include <vector>

#include "data_board_item.hxx"
#include "shape_line_chain.hxx"
#include "shape_poly_set.hxx"

namespace MINI {
	class DATA_PolySet : public BOARD_ITEM
	{
	public:
		DATA_PolySet(SHAPE_POLY_SET, double = 1);
		DATA_PolySet(SHAPE_POLY_SET, PCB_LAYER_ID aLayer, double = 1);

		virtual const BOX2I GetBoundingBox() const override;
        const SHAPE_POLY_SET& GetDrawPolySet(const VIEW* aView) const;

		std::string GetClass() const override {
			return "PolySet";
		}

		SHAPE_POLY_SET m_polySet;

		double m_lineWidth;

    private:
        mutable int            m_lodTolerance = -1;
        mutable SHAPE_POLY_SET m_lodPolySet;
	};
}
