#pragma once

#include "data_board_item.hxx"
#include "gal/include/line_segment_data.hxx"

#include <vector>

namespace MINI
{

class DATA_LineBatch : public BOARD_ITEM
{
public:
    DATA_LineBatch(PCB_LAYER_ID aLayer, const BOX2I& aTileBox, double aLineWidth = 1.0);

    void AddLine(const VECTOR2I& aStartPoint, const VECTOR2I& aEndPoint, int aSourceId = -1);
    void AddLine(const LineSegmentData& aLine);

    virtual const BOX2I GetBoundingBox() const override;
    std::string         GetClass() const override { return "LineBatch"; }

    const std::vector<LineSegmentData>& Lines() const { return m_lines; }
    double                              LineWidth() const { return m_lineWidth; }

private:
    BOX2I                        m_tileBox;
    double                       m_lineWidth;
    std::vector<LineSegmentData> m_lines;
};

} // namespace MINI
