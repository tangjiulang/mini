#include "data_line_batch.hxx"

using namespace MINI;

DATA_LineBatch::DATA_LineBatch(PCB_LAYER_ID aLayer, const BOX2I& aTileBox, double aLineWidth) :
        BOARD_ITEM(ITEM_TYPE::LINE_BATCH, aLayer),
        m_tileBox(aTileBox),
        m_lineWidth(aLineWidth)
{
}

void DATA_LineBatch::AddLine(const VECTOR2I& aStartPoint, const VECTOR2I& aEndPoint, int aSourceId)
{
    m_lines.push_back({ VECTOR2D(aStartPoint), VECTOR2D(aEndPoint), aSourceId });
}

void DATA_LineBatch::AddLine(const LineSegmentData& aLine)
{
    m_lines.push_back(aLine);
}

const BOX2I DATA_LineBatch::GetBoundingBox() const
{
    return m_tileBox;
}
