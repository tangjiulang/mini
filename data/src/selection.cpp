#include "selection.hxx"

bool SELECTION::operator==(const SELECTION& aOther) const
{
    return (m_items == aOther.m_items
            && m_itemsOrders == aOther.m_itemsOrders
            && m_isHover == aOther.m_isHover
            && m_lastAddedItem == aOther.m_lastAddedItem
            && m_orderCounter == aOther.m_orderCounter);
}


void SELECTION::Add(KIGFX::BOARD_ITEM* aItem)
{
    // We're not sorting here; this is just a time-optimized way to do an
    // inclusion check.  std::lower_bound will return the first i >= aItem
    // and the second i > aItem check rules out i == aItem.
    ITER i = std::lower_bound(m_items.begin(), m_items.end(), aItem);

    if (i == m_items.end() || *i > aItem)
    {
        m_itemsOrders.insert(m_itemsOrders.begin() + std::distance(m_items.begin(), i),
                             m_orderCounter);
        m_items.insert(i, aItem);
        m_orderCounter++;
        m_lastAddedItem = aItem;
    }
}


void SELECTION::Remove(KIGFX::BOARD_ITEM* aItem)
{
    ITER i = std::lower_bound(m_items.begin(), m_items.end(), aItem);

    if (!(i == m_items.end() || *i > aItem))
    {
        m_itemsOrders.erase(m_itemsOrders.begin() + std::distance(m_items.begin(), i));
        m_items.erase(i);

        if (aItem == m_lastAddedItem)
            m_lastAddedItem = nullptr;
    }
}


KIGFX::VIEW_ITEM* SELECTION::GetItem(unsigned int aIdx) const
{
    if (aIdx < m_items.size())
        return m_items[aIdx];

    return nullptr;
}


bool SELECTION::Contains(KIGFX::BOARD_ITEM* aItem) const
{
    CITER i = std::lower_bound(m_items.begin(), m_items.end(), aItem);

    return !(i == m_items.end() || *i > aItem);
}



BOX2I SELECTION::GetBoundingBox() const
{
    BOX2I bbox;

    for (KIGFX::BOARD_ITEM* item : m_items)
        bbox.Merge(item->GetBoundingBox());

    return bbox;
}




VECTOR2I SELECTION::GetReferencePoint() const
{
    if (m_referencePoint)
        return *m_referencePoint;
    else
        return GetBoundingBox().Centre();
}


void SELECTION::SetReferencePoint(const VECTOR2I& aP)
{
    m_referencePoint = aP;
}


void SELECTION::ClearReferencePoint()
{
    m_referencePoint = std::nullopt;
}


const std::vector<KIGFX::VIEW_ITEM*> SELECTION::updateDrawList() const
{
    std::vector<VIEW_ITEM*> items;

    for (KIGFX::BOARD_ITEM* item : m_items)
        items.push_back(item);

    return items;
}


bool SELECTION::AreAllItemsIdentical() const
{
    return std::all_of(m_items.begin() + 1, m_items.end(),
                       [&](const KIGFX::BOARD_ITEM* r)
                       {
                           return r->Type() == m_items.front()->Type();
                       });
}


std::vector<KIGFX::BOARD_ITEM*> SELECTION::GetItemsSortedBySelectionOrder() const
{
    using pairedIterators = std::pair<decltype(m_items.begin()),
        decltype(m_itemsOrders.begin())>;

    // Create a vector of all {selection item, selection order} iterator pairs
    std::vector<pairedIterators> pairs;
    auto                         item = m_items.begin();
    auto                         order = m_itemsOrders.begin();

    for (; item != m_items.end(); ++item, ++order)
        pairs.emplace_back(make_pair(item, order));

    // Sort the pairs by the selection order
    std::sort(pairs.begin(), pairs.end(),
              [](pairedIterators const& a, pairedIterators const& b)
              {
                  return *a.second < *b.second;
              });

    // Make a vector of just the sortedItems
    std::vector<KIGFX::BOARD_ITEM*> sortedItems;

    for (pairedIterators sortedItem : pairs)
        sortedItems.emplace_back(*sortedItem.first);

    return sortedItems;
}
