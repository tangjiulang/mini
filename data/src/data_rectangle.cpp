#include "data_rectangle.hxx"

using namespace MINI;

DATA_Rectangle::DATA_Rectangle(SHAPE_RECT rect, double lineWidth)
	: BOARD_ITEM(ITEM_TYPE::RECTANGLE),
	  m_rect(rect),
	  m_lineWidth(lineWidth) {}

MINI::DATA_Rectangle::DATA_Rectangle(SHAPE_RECT rect, PCB_LAYER_ID aLayer, double lineWidth)
	: BOARD_ITEM(ITEM_TYPE::RECTANGLE, aLayer),
	  m_rect(rect),
	  m_lineWidth(lineWidth) {}

const BOX2I DATA_Rectangle::GetBoundingBox() const
{
	return m_rect.BBox();
}
