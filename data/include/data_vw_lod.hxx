#pragma once

#include "shape_line_chain.hxx"
#include "shape_poly_set.hxx"

namespace MINI
{
class VIEW;

namespace DATA_VW_LOD
{
int GetTolerance(const VIEW* aView);

SHAPE_LINE_CHAIN SimplifyLineChain(const SHAPE_LINE_CHAIN& aLineChain, int aTolerance,
                                   bool aClosedOverride = false);

SHAPE_POLY_SET SimplifyPolySet(const SHAPE_POLY_SET& aPolySet, int aTolerance);
} // namespace DATA_VW_LOD
} // namespace MINI
