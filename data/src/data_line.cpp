#include "data_line.hxx"

using namespace MINI;

DATA_Line::DATA_Line(SHAPE_SEGMENT line, double lineWidth)
	: BOARD_ITEM(ITEM_TYPE::LINE),
	  m_line(line),
	  m_lineWidth(lineWidth) {}
MINI::DATA_Line::DATA_Line(SHAPE_SEGMENT line, PCB_LAYER_ID aLayer, double lineWidth)
	: BOARD_ITEM(ITEM_TYPE::LINE, aLayer),
	  m_line(line),
	  m_lineWidth(lineWidth) {}

const BOX2I DATA_Line::GetBoundingBox() const
{
	return m_line.BBox();
}
