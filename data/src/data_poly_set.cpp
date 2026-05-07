#include "data_poly_set.hxx"
#include "data_vw_lod.hxx"

MINI::DATA_PolySet::DATA_PolySet(SHAPE_POLY_SET polySet, double lineWidth)
	: BOARD_ITEM(ITEM_TYPE::POLYSET), m_polySet(polySet), m_lineWidth(lineWidth)
{}

MINI::DATA_PolySet::DATA_PolySet(SHAPE_POLY_SET polySet, PCB_LAYER_ID aLayer, double lineWidth)
	: BOARD_ITEM(ITEM_TYPE::POLYSET, aLayer), m_polySet(polySet), m_lineWidth(lineWidth)
{}

const BOX2I MINI::DATA_PolySet::GetBoundingBox() const
{
	return m_polySet.BBox();
}

const SHAPE_POLY_SET& MINI::DATA_PolySet::GetDrawPolySet(const VIEW* aView) const
{
    const int tolerance = DATA_VW_LOD::GetTolerance(aView);

    if (tolerance <= 0)
        return m_polySet;

    if (m_lodTolerance != tolerance)
    {
        m_lodPolySet = DATA_VW_LOD::SimplifyPolySet(m_polySet, tolerance);
        m_lodTolerance = tolerance;
    }

    return m_lodPolySet;
}
