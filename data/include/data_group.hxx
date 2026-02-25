#ifndef CLASS_PCB_GROUP_H_
#define CLASS_PCB_GROUP_H_

#include <data_board_item.hxx>
#include <unordered_set>

namespace MINI
{
    class VIEW;
}

/**
 * A set of BOARD_ITEMs (i.e., without duplicates).
 *
 * The group parent is always board, not logical parent group. The group is transparent
 * container - e.g., its position is derived from the position of its members.  A selection
 * containing a group implicitly contains its members. However other operations on sets of
 * items, like committing, updating the view, etc the set is explicit.
 */
class GROUP : public MINI::BOARD_ITEM
{
public:
    GROUP();
    GROUP(MINI::ITEM_TYPE idtype, PCB_LAYER_ID aLayer);


    BOARD_ITEM* AsEdaItem() { return this; }

    static inline bool ClassOf(const BOARD_ITEM* aItem)
    {
        return aItem && MINI::ITEM_TYPE::GROUP == aItem->Type();
    }

    std::string GetClass() const override
    {
        return "PCB_GROUP";
    }

    std::unordered_set<BOARD_ITEM*> GetBoardItems() const;

    /// @copydoc EDA_ITEM::GetBoundingBox
    const BOX2I GetBoundingBox() const override;


    /// @copydoc VIEW_ITEM::ViewGetLayers
    std::vector<int> ViewGetLayers() const override;


protected:

    std::unordered_set<BOARD_ITEM*> m_items;
};

#endif // CLASS_PCB_GROUP_H_
