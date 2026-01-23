#include "data_circle.hxx"

using namespace KIGFX;

DATA_Circle::DATA_Circle(SHAPE_CIRCLE circle, double lineWidth)
	: BOARD_ITEM(ITEM_TYPE::CIRCLE),
	  m_circle(circle),
	  m_lineWidth(lineWidth) {}

KIGFX::DATA_Circle::DATA_Circle(SHAPE_CIRCLE circle, PCB_LAYER_ID aLayer, double lineWidth)
	: BOARD_ITEM(ITEM_TYPE::CIRCLE, aLayer),
	  m_circle(circle),
	  m_lineWidth(lineWidth) {}

const BOX2I DATA_Circle::GetBoundingBox() const
{
	return m_circle.BBox();
}

