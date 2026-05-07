#include "data_line_batch_builder.hxx"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <unordered_map>

namespace
{
struct LineBatchKey
{
    int    layer = 0;
    int    tileX = 0;
    int    tileY = 0;
    double lineWidth = 1.0;

    bool operator==(const LineBatchKey& aOther) const
    {
        return layer == aOther.layer && tileX == aOther.tileX && tileY == aOther.tileY && lineWidth == aOther.lineWidth;
    }
};

struct LineBatchKeyHash
{
    size_t operator()(const LineBatchKey& aKey) const
    {
        size_t hash = std::hash<int>{}(aKey.layer);
        HashCombine(hash, aKey.tileX);
        HashCombine(hash, aKey.tileY);
        HashCombine(hash, aKey.lineWidth);
        return hash;
    }

    template <typename T>
    static void HashCombine(size_t& aSeed, const T& aValue)
    {
        aSeed ^= std::hash<T>{}(aValue) + 0x9e3779b9 + (aSeed << 6) + (aSeed >> 2);
    }
};

int FloorDiv(int aValue, int aDivisor)
{
    int quotient = aValue / aDivisor;
    int remainder = aValue % aDivisor;

    if(remainder != 0 && ((remainder < 0) != (aDivisor < 0)))
        --quotient;

    return quotient;
}

BOX2I MakeTileBox(int aTileX, int aTileY, int aTileSize)
{
    const int64_t minCoord = std::numeric_limits<int>::min();
    const int64_t maxCoord = std::numeric_limits<int>::max();
    const int64_t originX = static_cast<int64_t>(aTileX) * aTileSize;
    const int64_t originY = static_cast<int64_t>(aTileY) * aTileSize;
    const int64_t endX = originX + aTileSize;
    const int64_t endY = originY + aTileSize;

    return BOX2I::ByCorners(VECTOR2I(static_cast<int>(std::clamp(originX, minCoord, maxCoord)),
                                     static_cast<int>(std::clamp(originY, minCoord, maxCoord))),
                            VECTOR2I(static_cast<int>(std::clamp(endX, minCoord, maxCoord)),
                                     static_cast<int>(std::clamp(endY, minCoord, maxCoord))));
}

bool ClipLineToBox(const VECTOR2I& aStartPoint, const VECTOR2I& aEndPoint, const BOX2I& aBox, int aSourceId,
                   MINI::LineSegmentData& aClippedLine)
{
    BOX2I box = aBox;
    box.Normalize();

    const double minX = box.GetLeft();
    const double maxX = box.GetRight();
    const double minY = box.GetTop();
    const double maxY = box.GetBottom();
    const double x0 = aStartPoint.x;
    const double y0 = aStartPoint.y;
    const double x1 = aEndPoint.x;
    const double y1 = aEndPoint.y;
    const double dx = x1 - x0;
    const double dy = y1 - y0;

    double t0 = 0.0;
    double t1 = 1.0;

    auto clipEdge = [&](double aP, double aQ) -> bool
    {
        if(aP == 0.0)
            return aQ >= 0.0;

        const double r = aQ / aP;

        if(aP < 0.0)
        {
            if(r > t1)
                return false;

            t0 = std::max(t0, r);
            return true;
        }

        if(r < t0)
            return false;

        t1 = std::min(t1, r);
        return true;
    };

    if(!clipEdge(-dx, x0 - minX) || !clipEdge(dx, maxX - x0) || !clipEdge(-dy, y0 - minY) || !clipEdge(dy, maxY - y0)
       || t0 > t1)
    {
        return false;
    }

    const VECTOR2D clippedStart(x0 + t0 * dx, y0 + t0 * dy);
    const VECTOR2D clippedEnd(x0 + t1 * dx, y0 + t1 * dy);

    if(clippedStart == clippedEnd)
        return false;

    aClippedLine = { clippedStart, clippedEnd, aSourceId };
    return true;
}
} // namespace

std::vector<MINI::DATA_LineBatch> MINI::BuildLineBatches(const std::vector<DATA_Line>& aLines,
                                                         const LineBatchBuildOptions&  aOptions)
{
    const int tileSize = std::max(1, aOptions.tileSize);

    std::vector<DATA_LineBatch>                                batches;
    std::unordered_map<LineBatchKey, size_t, LineBatchKeyHash> batchIndices;

    batches.reserve(aLines.size() / 256 + 1);
    batchIndices.reserve(batches.capacity());

    for(size_t idx = 0; idx < aLines.size(); ++idx)
    {
        const DATA_Line& line = aLines[idx];
        std::vector<int> layers = line.ViewGetLayers();

        if(layers.empty())
            continue;

        BOX2I lineBox = line.GetBoundingBox();
        lineBox.Normalize();

        const int inflate = static_cast<int>(std::ceil(std::max(0.0, line.m_lineWidth * 0.5)));

        const int minTileX = FloorDiv(lineBox.GetLeft(), tileSize);
        const int maxTileX = FloorDiv(lineBox.GetRight(), tileSize);
        const int minTileY = FloorDiv(lineBox.GetTop(), tileSize);
        const int maxTileY = FloorDiv(lineBox.GetBottom(), tileSize);

        const SEG& segment = line.m_line.GetSeg();

        for(int tileX = minTileX; tileX <= maxTileX; ++tileX)
        {
            for(int tileY = minTileY; tileY <= maxTileY; ++tileY)
            {
                const BOX2I           tileBox = MakeTileBox(tileX, tileY, tileSize);
                MINI::LineSegmentData clippedLine;

                if(!ClipLineToBox(segment.A, segment.B, tileBox, static_cast<int>(idx), clippedLine))
                    continue;

                LineBatchKey key{ layers.front(), tileX, tileY, line.m_lineWidth };
                auto         it = batchIndices.find(key);

                if(it == batchIndices.end())
                {
                    const size_t batchIndex = batches.size();
                    // Keep geometry clipped to tileBox; inflate only the culling box for line width.
                    batches.emplace_back(static_cast<PCB_LAYER_ID>(key.layer), tileBox.GetInflated(inflate),
                                         line.m_lineWidth);
                    it = batchIndices.emplace(key, batchIndex).first;
                }

                batches[it->second].AddLine(clippedLine);
            }
        }
    }

    return batches;
}
