#include "data_vw_lod.hxx"

#include "view.hxx"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <queue>
#include <vector>

namespace MINI::DATA_VW_LOD
{
namespace
{
constexpr double kPixelTolerance = 1.0;
constexpr int kMinimumTolerance = 2;

int bucketizeTolerance(int aRawTolerance)
{
    if (aRawTolerance < kMinimumTolerance)
        return 0;

    int bucket = kMinimumTolerance;

    while (bucket <= aRawTolerance / 2)
        bucket <<= 1;

    return bucket;
}

struct QueueEntry
{
    double   area2;
    int      index;
    uint32_t version;
};

double triangleArea2(const VECTOR2I& a, const VECTOR2I& b, const VECTOR2I& c)
{
    const double abx = static_cast<double>(b.x) - a.x;
    const double aby = static_cast<double>(b.y) - a.y;
    const double acx = static_cast<double>(c.x) - a.x;
    const double acy = static_cast<double>(c.y) - a.y;
    return std::abs(abx * acy - aby * acx);
}

double squaredDistance(const VECTOR2I& a, const VECTOR2I& b)
{
    const double dx = static_cast<double>(b.x) - a.x;
    const double dy = static_cast<double>(b.y) - a.y;
    return dx * dx + dy * dy;
}

std::vector<VECTOR2I> filterDenseOpenPoints(const std::vector<VECTOR2I>& aPoints,
                                            int aDistanceThreshold,
                                            int aMinimumPointCount)
{
    if (aDistanceThreshold <= 0 || static_cast<int>(aPoints.size()) <= aMinimumPointCount)
        return aPoints;

    const double distanceThreshold2 =
        static_cast<double>(aDistanceThreshold) * static_cast<double>(aDistanceThreshold);
    std::vector<VECTOR2I> filtered;
    filtered.reserve(aPoints.size());
    filtered.push_back(aPoints.front());

    for (size_t idx = 1; idx + 1 < aPoints.size(); ++idx)
    {
        if (squaredDistance(filtered.back(), aPoints[idx]) < distanceThreshold2)
            continue;

        filtered.push_back(aPoints[idx]);
    }

    if (filtered.back() != aPoints.back())
    {
        if (filtered.size() > 1
            && squaredDistance(filtered.back(), aPoints.back()) < distanceThreshold2)
        {
            filtered.back() = aPoints.back();
        }
        else
        {
            filtered.push_back(aPoints.back());
        }
    }

    if (static_cast<int>(filtered.size()) < aMinimumPointCount || filtered.size() >= aPoints.size())
        return aPoints;

    return filtered;
}

std::vector<VECTOR2I> simplifyOpenPoints(const std::vector<VECTOR2I>& aPoints, double aAreaThreshold2,
                                         int aMinimumPointCount)
{
    if (static_cast<int>(aPoints.size()) <= aMinimumPointCount)
        return aPoints;

    struct Node
    {
        int      prev = -1;
        int      next = -1;
        bool     alive = false;
        uint32_t version = 0;
    };

    auto computeArea2 =
        [&](const std::vector<Node>& aNodes, int aIndex) -> double
        {
            const int prev = aNodes[aIndex].prev;
            const int next = aNodes[aIndex].next;

            if (prev < 0 || next < 0)
                return std::numeric_limits<double>::infinity();

            return triangleArea2(aPoints[prev], aPoints[aIndex], aPoints[next]);
        };

    auto isInterior =
        [&](const std::vector<Node>& aNodes, int aIndex) -> bool
        {
            return aNodes[aIndex].prev >= 0 && aNodes[aIndex].next >= 0;
        };

    std::vector<Node> nodes(aPoints.size());

    for (size_t idx = 0; idx < aPoints.size(); ++idx)
    {
        nodes[idx].prev = static_cast<int>(idx) - 1;
        nodes[idx].next = idx + 1 < aPoints.size() ? static_cast<int>(idx) + 1 : -1;
        nodes[idx].alive = true;
    }

    auto compare = [](const QueueEntry& lhs, const QueueEntry& rhs)
    {
        return lhs.area2 > rhs.area2;
    };

    std::priority_queue<QueueEntry, std::vector<QueueEntry>, decltype(compare)> queue(compare);

    auto enqueue =
        [&](int aIndex)
        {
            if (!isInterior(nodes, aIndex))
                return;

            const uint32_t version = ++nodes[aIndex].version;
            queue.push({ computeArea2(nodes, aIndex), aIndex, version });
        };

    for (int idx = 1; idx < static_cast<int>(aPoints.size()) - 1; ++idx)
        enqueue(idx);

    int aliveCount = static_cast<int>(aPoints.size());

    while (!queue.empty() && aliveCount > aMinimumPointCount)
    {
        const QueueEntry entry = queue.top();
        queue.pop();

        if (!nodes[entry.index].alive || nodes[entry.index].version != entry.version)
            continue;

        if (entry.area2 >= aAreaThreshold2)
            break;

        const int prev = nodes[entry.index].prev;
        const int next = nodes[entry.index].next;

        nodes[entry.index].alive = false;
        --aliveCount;

        if (prev >= 0)
            nodes[prev].next = next;

        if (next >= 0)
            nodes[next].prev = prev;

        if (prev >= 0)
            enqueue(prev);

        if (next >= 0)
            enqueue(next);
    }

    std::vector<VECTOR2I> result;
    result.reserve(aliveCount);

    for (int idx = 0; idx >= 0 && idx < static_cast<int>(nodes.size()); idx = nodes[idx].next)
    {
        if (nodes[idx].alive)
            result.push_back(aPoints[idx]);
    }

    return result;
}

std::vector<VECTOR2I> rotateToMostSignificantVertex(const std::vector<VECTOR2I>& aPoints)
{
    if (aPoints.size() < 3)
        return aPoints;

    size_t bestIndex = 0;
    double bestArea2 = -1.0;

    for (size_t idx = 0; idx < aPoints.size(); ++idx)
    {
        const size_t prev = (idx + aPoints.size() - 1) % aPoints.size();
        const size_t next = (idx + 1) % aPoints.size();
        const double area2 = triangleArea2(aPoints[prev], aPoints[idx], aPoints[next]);

        if (area2 > bestArea2)
        {
            bestArea2 = area2;
            bestIndex = idx;
        }
    }

    std::vector<VECTOR2I> rotated;
    rotated.reserve(aPoints.size());

    for (size_t offset = 0; offset < aPoints.size(); ++offset)
        rotated.push_back(aPoints[(bestIndex + offset) % aPoints.size()]);

    return rotated;
}

std::vector<VECTOR2I> simplifyClosedPoints(const std::vector<VECTOR2I>& aPoints, double aAreaThreshold2)
{
    if (aPoints.size() < 4)
        return aPoints;

    std::vector<VECTOR2I> rotated = rotateToMostSignificantVertex(aPoints);
    rotated.push_back(rotated.front());

    std::vector<VECTOR2I> simplified = simplifyOpenPoints(rotated, aAreaThreshold2, 4);

    if (!simplified.empty() && simplified.front() == simplified.back())
        simplified.pop_back();

    if (simplified.size() < 3)
        return aPoints;

    return simplified;
}

std::vector<VECTOR2I> filterDenseClosedPoints(const std::vector<VECTOR2I>& aPoints,
                                              int aDistanceThreshold)
{
    if (aDistanceThreshold <= 0 || aPoints.size() < 4)
        return aPoints;

    std::vector<VECTOR2I> rotated = rotateToMostSignificantVertex(aPoints);
    rotated.push_back(rotated.front());

    std::vector<VECTOR2I> filtered = filterDenseOpenPoints(rotated, aDistanceThreshold, 4);

    if (!filtered.empty() && filtered.front() == filtered.back())
        filtered.pop_back();

    if (filtered.size() < 3 || filtered.size() >= aPoints.size())
        return aPoints;

    return filtered;
}

SHAPE_LINE_CHAIN makeLineChain(const std::vector<VECTOR2I>& aPoints, bool aClosed, int aWidth)
{
    SHAPE_LINE_CHAIN result;
    result.ReservePoints(aPoints.size());
    result.SetWidth(aWidth);

    for (const VECTOR2I& point : aPoints)
        result.Append(point);

    result.SetClosed(aClosed);
    return result;
}
} // namespace

int GetTolerance(const VIEW* aView)
{
    if (!aView || aView->GetPainter()->GetSettings()->IsPrinting())
        return 0;

    const double worldPixel = std::abs(aView->ToWorld(kPixelTolerance));
    const int rawTolerance = static_cast<int>(std::ceil(worldPixel));
    return bucketizeTolerance(rawTolerance);
}

SHAPE_LINE_CHAIN SimplifyLineChain(const SHAPE_LINE_CHAIN& aLineChain, int aTolerance,
                                   bool aClosedOverride)
{
    const bool isClosed = aClosedOverride || aLineChain.IsClosed();
    const int minimumPointCount = isClosed ? 3 : 2;

    if (aTolerance <= 0 || aLineChain.PointCount() <= minimumPointCount)
        return aLineChain;

    const auto& pointsRef = aLineChain.CPoints();
    std::vector<VECTOR2I> points(pointsRef.begin(), pointsRef.end());

    if (static_cast<int>(points.size()) <= minimumPointCount)
        return aLineChain;

    const std::vector<VECTOR2I> prefiltered =
        isClosed ? filterDenseClosedPoints(points, aTolerance)
                 : filterDenseOpenPoints(points, aTolerance, minimumPointCount);
    const std::vector<VECTOR2I>& basePoints =
        prefiltered.size() < points.size() ? prefiltered : points;

    if (static_cast<int>(basePoints.size()) <= minimumPointCount)
        return aLineChain;

    const double areaThreshold2 = static_cast<double>(aTolerance) * aTolerance;
    std::vector<VECTOR2I> simplified =
        isClosed ? simplifyClosedPoints(basePoints, areaThreshold2)
                 : simplifyOpenPoints(basePoints, areaThreshold2, minimumPointCount);

    if (static_cast<int>(simplified.size()) < minimumPointCount || simplified.size() >= basePoints.size())
        simplified = basePoints;

    if (simplified.size() >= points.size())
        return aLineChain;

    return makeLineChain(simplified, isClosed, aLineChain.Width());
}

SHAPE_POLY_SET SimplifyPolySet(const SHAPE_POLY_SET& aPolySet, int aTolerance)
{
    if (aTolerance <= 0 || aPolySet.IsEmpty())
        return aPolySet;

    SHAPE_POLY_SET result;

    for (int outlineIdx = 0; outlineIdx < aPolySet.OutlineCount(); ++outlineIdx)
    {
        SHAPE_LINE_CHAIN simplifiedOutline = SimplifyLineChain(aPolySet.COutline(outlineIdx), aTolerance, true);

        if (simplifiedOutline.PointCount() < 3)
            continue;

        const int newOutlineIdx = result.AddOutline(simplifiedOutline);

        for (int holeIdx = 0; holeIdx < aPolySet.HoleCount(outlineIdx); ++holeIdx)
        {
            SHAPE_LINE_CHAIN simplifiedHole = SimplifyLineChain(aPolySet.CHole(outlineIdx, holeIdx), aTolerance, true);

            if (simplifiedHole.PointCount() >= 3)
                result.AddHole(simplifiedHole, newOutlineIdx);
        }
    }

    if (result.OutlineCount() == 0)
        return aPolySet;

    result.Simplify();
    return result;
}
} // namespace MINI::DATA_VW_LOD
