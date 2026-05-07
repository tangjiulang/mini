#include <algorithm>
#include <array>
#include <cmath>
#include <numbers>
#include <spdlog/spdlog.h>

#include "data_manager.hxx"
#include "view.hxx"

namespace
{
enum class TestDataSet
{
    LineBatch,
    Polygon
};

constexpr TestDataSet kActiveTestDataSet = TestDataSet::LineBatch;

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
        edges[idx] = SEG(vertices[idx], vertices[(idx + 1) % vertices.size()]);

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
    m_lineBatches.clear();
    m_rectangles.clear();
    m_triangles.clear();
    m_polylines.clear();
    m_polygons.clear();
    m_arcs.clear();
    m_polySet.clear();
    m_texts.clear();

    switch(kActiveTestDataSet)
    {
    case TestDataSet::Polygon: GeneratePolygonTestData(view); break;
    case TestDataSet::LineBatch:
    default: GenerateLineBatchTestData(view); break;
    }

    spdlog::info("GenerateData done");
}

void DataManager::GenerateLineBatchTestData(MINI::VIEW* aView)
{
    spdlog::info("GenerateLineBatchTestData start");

    constexpr int kGroupRows = 50;
    constexpr int kGroupCols = 50;
    constexpr int kRoutesPerGroup = 100;
    constexpr int kSegmentsPerRoute = 4;
    constexpr int kTargetLineCount = kGroupRows * kGroupCols * kRoutesPerGroup * kSegmentsPerRoute;
    static_assert(kTargetLineCount == 1000000);

    constexpr double kGroupWidth = 700.0;
    constexpr double kGroupHeight = 1300.0;
    constexpr double kRouteMarginX = 42.0;
    constexpr double kRouteMarginY = 44.0;
    constexpr double kTracePitch = 9.0;
    constexpr double kPadEscape = 36.0;
    constexpr double kDiagonalJog = 24.0;
    constexpr double kMainRun = 420.0;
    constexpr double kExitRun = 36.0;
    constexpr double kSerpentineLength = 72.0;

    const std::array<double, 4> lineWidths = {
        static_cast<double>(ToWorldLength(aView, 1.2)),
        static_cast<double>(ToWorldLength(aView, 1.6)),
        static_cast<double>(ToWorldLength(aView, 2.0)),
        static_cast<double>(ToWorldLength(aView, 2.8)),
    };

    auto toWorldOffset = [&](double aX, double aY)
    {
        return aView->ToWorld({ aX, aY }, false);
    };

    const VECTOR2I screenPixelSize = aView->GetScreenPixelSize();
    const VECTOR2D screenCenter(std::max(1000, screenPixelSize.x) * 0.5, std::max(1000, screenPixelSize.y) * 0.5);
    const VECTOR2D worldCenter = aView->ToWorld(screenCenter);
    const VECTOR2D worldStart =
            worldCenter - toWorldOffset(kGroupCols * kGroupWidth * 0.5, kGroupRows * kGroupHeight * 0.5);

    m_lines.reserve(kTargetLineCount);

    auto addLine = [&](const VECTOR2D& aStartPoint, const VECTOR2D& aEndPoint, PCB_LAYER_ID aLayer, double aLineWidth)
    {
        m_lines.emplace_back(SHAPE_SEGMENT(ToVector2I(aStartPoint), ToVector2I(aEndPoint)), aLayer, aLineWidth);
    };

    // Deterministic PCB-like routes: parallel buses, doglegs, 45-degree fanout, and tuning bumps.
    for(int groupRow = 0; groupRow < kGroupRows; ++groupRow)
    {
        if(groupRow % 5 == 0)
            spdlog::info("GenerateLineBatchTestData group row {} / {}", groupRow, kGroupRows);

        for(int groupCol = 0; groupCol < kGroupCols; ++groupCol)
        {
            const int      groupIndex = groupRow * kGroupCols + groupCol;
            const double   direction = ((groupRow + groupCol) % 2 == 0) ? 1.0 : -1.0;
            const double   startX = direction > 0.0 ? kRouteMarginX : kGroupWidth - kRouteMarginX;
            const VECTOR2D groupOrigin = worldStart + toWorldOffset(groupCol * kGroupWidth, groupRow * kGroupHeight);

            for(int route = 0; route < kRoutesPerGroup; ++route)
            {
                const int          routeIndex = groupIndex * kRoutesPerGroup + route;
                const PCB_LAYER_ID layer = (routeIndex % 6 == 0) ? B_Cu : F_Cu;
                const double   lineWidth = lineWidths[static_cast<size_t>(routeIndex + groupCol) % lineWidths.size()];
                const double   laneY = kRouteMarginY + route * kTracePitch + ((route % 2 == 0) ? 0.0 : 2.0);
                const double   padStagger = static_cast<double>(route % 8) * 1.5;
                const double   jogMagnitude = kDiagonalJog + static_cast<double>((route / 13) % 3) * 4.0;
                const double   jogY = ((route + groupCol) % 2 == 0) ? jogMagnitude : -jogMagnitude;
                const double   mainRun = kMainRun + static_cast<double>((route + groupRow) % 7) * 12.0;
                const VECTOR2D routeStart = groupOrigin + toWorldOffset(startX + direction * padStagger, laneY);

                std::array<VECTOR2D, 5> points = { routeStart, routeStart, routeStart, routeStart, routeStart };

                switch((route + groupIndex) % 4)
                {
                case 0:
                    points[1] = points[0] + toWorldOffset(direction * kPadEscape, 0.0);
                    points[2] = points[1] + toWorldOffset(direction * jogMagnitude, jogY);
                    points[3] = points[2] + toWorldOffset(direction * mainRun, 0.0);
                    points[4] = points[3] + toWorldOffset(direction * jogMagnitude, -jogY);
                    break;
                case 1:
                    points[1] = points[0] + toWorldOffset(direction * (kPadEscape + 10.0), 0.0);
                    points[2] = points[1] + toWorldOffset(0.0, jogY);
                    points[3] = points[2] + toWorldOffset(direction * (mainRun + kExitRun), 0.0);
                    points[4] = points[3] + toWorldOffset(0.0, -jogY);
                    break;
                case 2:
                    points[1] = points[0] + toWorldOffset(direction * kPadEscape, 0.0);
                    points[2] = points[1] + toWorldOffset(direction * jogMagnitude, jogY);
                    points[3] = points[2] + toWorldOffset(direction * jogMagnitude, -jogY);
                    points[4] = points[3] + toWorldOffset(direction * mainRun, 0.0);
                    break;
                default:
                    points[1] = points[0] + toWorldOffset(direction * (mainRun * 0.35), 0.0);
                    points[2] = points[1] + toWorldOffset(direction * jogMagnitude, jogY);
                    points[3] = points[2] + toWorldOffset(direction * (kSerpentineLength + (route % 5) * 4.0), 0.0);
                    points[4] = points[3] + toWorldOffset(direction * jogMagnitude, -jogY);
                    break;
                }

                for(size_t idx = 0; idx < points.size() - 1; ++idx)
                    addLine(points[idx], points[idx + 1], layer, lineWidth);
            }
        }
    }

    spdlog::info("GenerateLineBatchTestData done with {} lines (target {})", m_lines.size(), kTargetLineCount);
}

void DataManager::GeneratePolygonTestData(MINI::VIEW* aView)
{
    spdlog::info("GeneratePolygonTestData start");

    constexpr int    kRows = 1000;
    constexpr int    kCols = 1000;
    constexpr double kRadius = 18.0;
    constexpr double kCornerRadius = 2.5;
    constexpr double kLineWidth = 2.0;
    constexpr double kGapX = 18.0;
    constexpr double kGapY = 14.0;

    const int                        lineWidth = ToWorldLength(aView, kLineWidth);
    const std::vector<MINI::Segment> hexagonTemplate =
            MakeRoundedHexagonTemplate(aView, kRadius, kCornerRadius, lineWidth);

    const VECTOR2I screenPixelSize = aView->GetScreenPixelSize();
    const VECTOR2D screenCenter(std::max(1000, screenPixelSize.x) * 0.5, std::max(1000, screenPixelSize.y) * 0.5);
    const VECTOR2D worldCenter = aView->ToWorld(screenCenter);
    const double   hexHalfWidth = kRadius * std::cos(std::numbers::pi_v<double> / 6.0);
    const double   stepX = 2.0 * hexHalfWidth + kGapX;
    const double   stepY = kRadius * 1.5 + kGapY;
    const VECTOR2D worldStepX = aView->ToWorld({ stepX, 0.0 }, false);
    const VECTOR2D worldStepY = aView->ToWorld({ 0.0, stepY }, false);
    const VECTOR2D worldRowOffset = aView->ToWorld({ stepX * 0.5, 0.0 }, false);
    const VECTOR2D worldStart = worldCenter - worldStepX * (static_cast<double>(kCols - 1) * 0.5)
                                - worldStepY * (static_cast<double>(kRows - 1) * 0.5);

    m_polygons.reserve(static_cast<size_t>(kRows) * static_cast<size_t>(kCols));

    for(int row = 0; row < kRows; ++row)
    {
        if(row % 100 == 0)
            spdlog::info("GeneratePolygonTestData row {} / {}", row, kRows);

        const VECTOR2D rowBase = worldStart + worldStepY * row + ((row % 2 == 0) ? VECTOR2D() : worldRowOffset);

        for(int col = 0; col < kCols; ++col)
        {
            const VECTOR2I center = ToVector2I(rowBase + worldStepX * col);

            MINI::DATA_Polygon polygon =
                    MakeRoundedHexagonPolygon(hexagonTemplate, center, static_cast<double>(lineWidth));
            m_polygons.push_back(std::move(polygon));
        }
    }

    spdlog::info("GeneratePolygonTestData done with {} polygons", m_polygons.size());
}
