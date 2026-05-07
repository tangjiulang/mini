#pragma once

#include "box2.hxx"
#include "layer_ids.hxx"
#include "vector2d.hxx"

#include <vector>

namespace MINI
{

struct RoutingSegment
{
    VECTOR2I     start;
    VECTOR2I     end;
    PCB_LAYER_ID layer = F_Cu;
    double       width = 1.0;
    double       clearance = 0.0;
    int          sourceId = -1;
};

class RoutingObstacleIndex
{
public:
    virtual ~RoutingObstacleIndex() = default;

    virtual void Clear() = 0;
    virtual void AddSegment(const RoutingSegment& aSegment) = 0;
    virtual void Query(const BOX2I& aBounds, std::vector<int>& aResultSourceIds) const = 0;
};

} // namespace MINI
