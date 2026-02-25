#pragma once

#include <deque>
#include "view_group.hxx"
#include "data_board_item.hxx"


class SELECTION : public MINI::VIEW_GROUP
{
public:
    SELECTION() :
        MINI::VIEW_GROUP::VIEW_GROUP()
    {
        m_isHover = false;
        m_lastAddedItem = nullptr;
        m_orderCounter = 0;
    }

    SELECTION(const SELECTION& aOther) :
        MINI::VIEW_GROUP::VIEW_GROUP()
    {
        m_items = aOther.m_items;
        m_itemsOrders = aOther.m_itemsOrders;
        m_isHover = aOther.m_isHover;
        m_lastAddedItem = aOther.m_lastAddedItem;
        m_orderCounter = aOther.m_orderCounter;
    }

    SELECTION& operator= (const SELECTION& aOther)
    {
        m_items = aOther.m_items;
        m_itemsOrders = aOther.m_itemsOrders;
        m_isHover = aOther.m_isHover;
        m_lastAddedItem = aOther.m_lastAddedItem;
        m_orderCounter = aOther.m_orderCounter;
        return *this;
    }

    std::string GetClass() const override
    {
        return std::string("SELECTION");
    }

    bool operator==(const SELECTION& aOther) const;

    using ITER = std::deque<MINI::BOARD_ITEM*>::iterator;
    using CITER = std::deque<MINI::BOARD_ITEM*>::const_iterator;

    ITER begin() { return m_items.begin(); }
    ITER end() { return m_items.end(); }
    CITER begin() const { return m_items.cbegin(); }
    CITER end() const { return m_items.cend(); }

    void SetIsHover(bool aIsHover)
    {
        m_isHover = aIsHover;
    }

    bool IsHover() const
    {
        return m_isHover;
    }

    virtual void Add(MINI::BOARD_ITEM* aItem);

    virtual void Remove(MINI::BOARD_ITEM* aItem);

    virtual void Clear() override
    {
        m_items.clear();
        m_itemsOrders.clear();
        m_orderCounter = 0;
    }

    virtual unsigned int GetSize() const override
    {
        return m_items.size();
    }

    virtual MINI::VIEW_ITEM* GetItem(unsigned int aIdx) const override;

    bool Contains(MINI::BOARD_ITEM* aItem) const;

    /// Checks if there is anything selected
    bool Empty() const
    {
        return m_items.empty();
    }

    /// Returns the number of selected parts
    int Size() const
    {
        return m_items.size();
    }

    const std::deque<MINI::BOARD_ITEM*> GetItems() const
    {
        return m_items;
    }

    MINI::BOARD_ITEM* GetLastAddedItem() const
    {
        return m_lastAddedItem;
    }

    std::vector<MINI::BOARD_ITEM*> GetItemsSortedBySelectionOrder() const;


    virtual const BOX2I ViewBBox() const override
    {
        BOX2I r;
        r.SetMaximum();
        return r;
    }

    /// Returns the top left point of the selection area bounding box.
    VECTOR2I GetPosition() const
    {
        return GetBoundingBox().GetPosition();
    }

    virtual BOX2I GetBoundingBox() const;

    virtual MINI::BOARD_ITEM* GetTopLeftItem(bool onlyModules = false) const
    {
        return nullptr;
    }

    MINI::BOARD_ITEM* operator[](const size_t aIdx) const
    {
        if (aIdx < m_items.size())
            return m_items[aIdx];

        return nullptr;
    }

    MINI::BOARD_ITEM* Front() const
    {
        return m_items.size() ? m_items.front() : nullptr;
    }

    std::deque<MINI::BOARD_ITEM*>& Items()
    {
        return m_items;
    }

    const std::deque<MINI::BOARD_ITEM*>& Items() const
    {
        return m_items;
    }

    template<class T>
    T* FirstOfKind() const
    {
        for (auto item : m_items)
        {
            if (IsA<T, MINI::BOARD_ITEM>(item))
                return static_cast<T*> (item);
        }

        return nullptr;
    }


    virtual const std::vector<MINI::VIEW_ITEM*> updateDrawList() const override;

    bool HasReferencePoint() const
    {
        return m_referencePoint != std::nullopt;
    }

    VECTOR2I GetReferencePoint() const;
    void SetReferencePoint(const VECTOR2I& aP);
    void ClearReferencePoint();

    /**
     * Checks if all items in the selection are the same KICAD_T type
     *
     * @return True if all items are the same type, this includes zero or single items
     */
    bool AreAllItemsIdentical() const;


protected:
    std::optional<VECTOR2I>         m_referencePoint;
    std::deque<MINI::BOARD_ITEM*> m_items;
    std::deque<int>       m_itemsOrders;
    int                   m_orderCounter;
    MINI::BOARD_ITEM*    m_lastAddedItem;
    bool                  m_isHover;

    // mute hidden overloaded virtual function warnings
    using VIEW_GROUP::Add;
    using VIEW_GROUP::Remove;
};
