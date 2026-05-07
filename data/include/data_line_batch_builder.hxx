#pragma once

#include "data_line.hxx"
#include "data_line_batch.hxx"

#include <vector>

namespace MINI
{

struct LineBatchBuildOptions
{
    int tileSize = 500000000;
};

std::vector<DATA_LineBatch> BuildLineBatches(const std::vector<DATA_Line>& aLines,
                                             const LineBatchBuildOptions&  aOptions = {});

} // namespace MINI
