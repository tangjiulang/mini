#include "data_line.hxx"

using namespace KIGFX;

DATA_Line::DATA_Line(SHAPE_SEGMENT line, double lineWidth)
	: BOARD_ITEM(ITEM_TYPE::LINE),
	  m_line(line),
	  m_lineWidth(lineWidth) {}

const BOX2I DATA_Line::GetBoundingBox() const
{
	return m_line.BBox();
}
