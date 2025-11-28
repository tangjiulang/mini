#include <vector>
#include "vector2d.hxx"

// ------------------ 4. 分类 CW 外框 / CCW 洞 ------------------
struct ContourResult {
    std::vector<std::vector<VECTOR2D>> outer;
    std::vector<std::vector<VECTOR2D>> holes;
};


ContourResult scanline_to_polygons(const std::vector<VECTOR2D>& poly, double dy = 0.5, double simplify_eps = 1e-3);