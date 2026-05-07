#pragma once

#include "vector2d.hxx"

namespace MINI
{

struct LineSegmentData
{
    VECTOR2D start;
    VECTOR2D end;
    int      sourceId = -1;
};

} // namespace MINI
