#include "data_group.hxx"

#include <shape_compound.hxx>
#include <data_board_item.hxx>
#include <view.hxx>


GROUP::GROUP() :
    BOARD_ITEM(MINI::ITEM_TYPE::GROUP)
{
}


GROUP::GROUP(MINI::ITEM_TYPE idtype, PCB_LAYER_ID aLayer) :
    BOARD_ITEM(idtype, aLayer)
{
}

std::unordered_set<MINI::BOARD_ITEM*> GROUP::GetBoardItems() const
{
    std::unordered_set<BOARD_ITEM*> items;

    for (BOARD_ITEM* item : m_items)
    {
        if (item->IsBOARD_ITEM())
            items.insert(static_cast<BOARD_ITEM*>(item));
    }

    return items;
}

std::vector<int> GROUP::ViewGetLayers() const
{
    return { LAYER_ANCHOR };
}
