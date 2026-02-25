#include "data_poly_set.hxx"

MINI::DATA_PolySet::DATA_PolySet(SHAPE_POLY_SET polySet, double lineWidth)
	: BOARD_ITEM(ITEM_TYPE::POLYSET), m_polySet(polySet), m_lineWidth(lineWidth)
{}

MINI::DATA_PolySet::DATA_PolySet(SHAPE_POLY_SET, PCB_LAYER_ID aLayer, double lineWidth)
	: BOARD_ITEM(ITEM_TYPE::POLYSET, aLayer), m_polySet(m_polySet), m_lineWidth(lineWidth)
{}

const BOX2I MINI::DATA_PolySet::GetBoundingBox() const
{
	return m_polySet.BBox();
}
