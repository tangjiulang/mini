#include "data_arc.hxx"

KIGFX::DATA_Arc::DATA_Arc(SHAPE_ARC arc, double lineWidth)
	: BOARD_ITEM(ITEM_TYPE::ARC), m_arc(arc), m_lineWidth(lineWidth)
{}

KIGFX::DATA_Arc::DATA_Arc(SHAPE_ARC arc, PCB_LAYER_ID aLayer, double lineWidth)
	: BOARD_ITEM(ITEM_TYPE::ARC, aLayer), m_arc(arc), m_lineWidth(lineWidth)
{}

const BOX2I KIGFX::DATA_Arc::GetBoundingBox() const
{
	return m_arc.BBox();
}
