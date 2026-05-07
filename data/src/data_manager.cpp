#include <algorithm>
#include <array>
#include <cmath>
#include <numbers>
#include <spdlog/spdlog.h>

#include "data_manager.hxx"
#include "view.hxx"

namespace
{
VECTOR2I ToVector2I(const VECTOR2D& aPoint)
{
    return { static_cast<int>(std::lround(aPoint.x)), static_cast<int>(std::lround(aPoint.y)) };
}

int ToWorldLength(MINI::VIEW* aView, double aScreenLength)
{
    return std::max(1, static_cast<int>(std::lround(std::abs(aView->ToWorld(aScreenLength)))));
}

std::vector<MINI::Segment> MakeRoundedHexagonTemplate(MINI::VIEW* aView, double aScreenRadius,
                                                      double aScreenCornerRadius, int aLineWidth)
{
    const int cornerRadius = ToWorldLength(aView, aScreenCornerRadius);

    std::array<VECTOR2I, 6> vertices;

    for(size_t idx = 0; idx < vertices.size(); ++idx)
    {
        const double angle =
                std::numbers::pi_v<double> / 6.0 + (std::numbers::pi_v<double> / 3.0) * static_cast<double>(idx);
        const VECTOR2D worldOffset =
                aView->ToWorld({ aScreenRadius * std::cos(angle), aScreenRadius * std::sin(angle) }, false);
        vertices[idx] = ToVector2I(worldOffset);
    }

    std::array<SEG, 6> edges;

    for(size_t idx = 0; idx < edges.size(); ++idx)
    {
        edges[idx] = SEG(vertices[idx], vertices[(idx + 1) % vertices.size()]);
    }

    std::array<SHAPE_ARC, 6> corners;

    for(size_t idx = 0; idx < corners.size(); ++idx)
    {
        const size_t previous = (idx + corners.size() - 1) % corners.size();
        corners[idx] = SHAPE_ARC(edges[previous], edges[idx], cornerRadius, aLineWidth);
    }

    std::vector<MINI::Segment> segments;
    segments.reserve(12);

    for(size_t idx = 0; idx < corners.size(); ++idx)
    {
        corners[idx].SetWidth(aLineWidth);
        segments.emplace_back(corners[idx]);

        const VECTOR2I lineStart = corners[idx].GetP1();
        const VECTOR2I lineEnd = corners[(idx + 1) % corners.size()].GetP0();

        if(lineStart != lineEnd)
            segments.emplace_back(SHAPE_SEGMENT(lineStart, lineEnd, aLineWidth));
    }

    return segments;
}

MINI::Segment OffsetSegment(const MINI::Segment& aSegment, const VECTOR2I& aOffset)
{
    if(std::holds_alternative<SHAPE_ARC>(aSegment))
    {
        SHAPE_ARC arc = std::get<SHAPE_ARC>(aSegment);
        arc.Move(aOffset);
        return arc;
    }

    SHAPE_SEGMENT segment = std::get<SHAPE_SEGMENT>(aSegment);
    segment.Move(aOffset);
    return segment;
}

MINI::DATA_Polygon MakeRoundedHexagonPolygon(const std::vector<MINI::Segment>& aTemplateSegments,
                                             const VECTOR2I& aCenter, double aLineWidth)
{
    std::vector<MINI::Segment> segments;
    segments.reserve(aTemplateSegments.size());

    for(const MINI::Segment& templateSegment : aTemplateSegments)
        segments.emplace_back(OffsetSegment(templateSegment, aCenter));

    return { std::move(segments), aLineWidth };
}
} // namespace

void DataManager::GenerateData(MINI::VIEW* view)
{
    spdlog::info("GenerateData start");
    m_circles.clear();
    m_lines.clear();
    m_rectangles.clear();
    m_triangles.clear();
    m_polylines.clear();
    m_polygons.clear();
    m_arcs.clear();
    m_polySet.clear();
    m_texts.clear();

    constexpr int    kRows = 1000;
    constexpr int    kCols = 1000;
    constexpr double kRadius = 18.0;
    constexpr double kCornerRadius = 2.5;
    constexpr double kLineWidth = 2.0;
    constexpr double kGapX = 18.0;
    constexpr double kGapY = 14.0;

    const int                        lineWidth = ToWorldLength(view, kLineWidth);
    const std::vector<MINI::Segment> hexagonTemplate =
            MakeRoundedHexagonTemplate(view, kRadius, kCornerRadius, lineWidth);

    const VECTOR2I screenPixelSize = view->GetScreenPixelSize();
    const VECTOR2D screenCenter(std::max(1000, screenPixelSize.x) * 0.5, std::max(1000, screenPixelSize.y) * 0.5);
    const VECTOR2D worldCenter = view->ToWorld(screenCenter);
    const double   hexHalfWidth = kRadius * std::cos(std::numbers::pi_v<double> / 6.0);
    const double   stepX = 2.0 * hexHalfWidth + kGapX;
    const double   stepY = kRadius * 1.5 + kGapY;
    const VECTOR2D worldStepX = view->ToWorld({ stepX, 0.0 }, false);
    const VECTOR2D worldStepY = view->ToWorld({ 0.0, stepY }, false);
    const VECTOR2D worldRowOffset = view->ToWorld({ stepX * 0.5, 0.0 }, false);
    const VECTOR2D worldStart = worldCenter - worldStepX * (static_cast<double>(kCols - 1) * 0.5)
                                - worldStepY * (static_cast<double>(kRows - 1) * 0.5);

    m_polygons.reserve(static_cast<size_t>(kRows) * static_cast<size_t>(kCols));

    // Generate a dispersed million-item honeycomb of rounded regular
    // hexagons for large-scale VW and culling tests.
    for(int row = 0; row < kRows; ++row)
    {
        if(row % 100 == 0)
            spdlog::info("GenerateData row {} / {}", row, kRows);

        const VECTOR2D rowBase = worldStart + worldStepY * row + ((row % 2 == 0) ? VECTOR2D() : worldRowOffset);

        for(int col = 0; col < kCols; ++col)
        {
            const VECTOR2I center = ToVector2I(rowBase + worldStepX * col);

            MINI::DATA_Polygon polygon =
                    MakeRoundedHexagonPolygon(hexagonTemplate, center, static_cast<double>(lineWidth));
            m_polygons.push_back(std::move(polygon));
        }
    }

    spdlog::info("GenerateData done with {} polygons", m_polygons.size());
}
