#include "data_board_item.hxx"

using namespace KIGFX;

const BOX2I BOARD_ITEM::ViewBBox() const
{
    // Basic fallback
    return GetBoundingBox();
}

const BOX2I BOARD_ITEM::GetBoundingBox() const
{
    // return a zero-sized box per default. derived classes should override
    // this
    return BOX2I(VECTOR2I(0, 0), VECTOR2I(0, 0));
}

std::vector<int> BOARD_ITEM::ViewGetLayers() const
{
    return { m_layer };
}