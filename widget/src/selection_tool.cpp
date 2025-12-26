#include "data_item_flags.hxx"
#include "selection_tool.hxx"
#include "selection_area.hxx"

using namespace KIGFX;

constexpr int MIN_INT = std::numeric_limits<int>::min();


SELECTION_TOOL::SELECTION_TOOL()
{
    m_area = new SELECTION_AREA();
	m_view = nullptr;
	m_area->SetOrigin(VECTOR2I(MIN_INT, MIN_INT));
    m_area->SetEnd(VECTOR2I(MIN_INT, MIN_INT));
}

SELECTION_TOOL::~SELECTION_TOOL()
{
    delete(m_area);
}

SELECTION& SELECTION_TOOL::GetSelection() {
    return m_selection;
}

void SELECTION_TOOL::ClearSelection(bool aQuietMode)
{
    if (m_selection.Empty())
        return;

    while (m_selection.GetSize())
        unhighlight(m_selection.Front(), SELECTED, &m_selection);

    m_view->Update(&m_selection);
}

void SELECTION_TOOL::SetOrigin(const VECTOR2I& aOrigin) {
    m_view->SetVisible(m_area, true);
    ClearSelection();
    m_area->SetOrigin(aOrigin);
}

void SELECTION_TOOL::SetEnd(const VECTOR2I& aEnd) {
    m_area->SetEnd(aEnd);
}

int SELECTION_TOOL::SelectRectArea() {
    m_view->SetVisible(m_area, false);
    m_view->Remove(&m_selection);
    m_view->Add(&m_selection);

	BOX2I selectionBox = m_area->GetBoundingBox();
    std::vector<VIEW::LAYER_ITEM_PAIR> candidates;
    m_view->Query(selectionBox, candidates);

    std::vector<BOARD_ITEM*> items;
    for (const auto& [item, layer] : candidates) {
        if (!item->IsBOARD_ITEM())
            continue;

		auto boardItem = static_cast<BOARD_ITEM*>(item);

        items.push_back(boardItem);
    }


    for (auto item : items)
        select(item);
    return 0;
}

void SELECTION_TOOL::SetView(KIGFX::VIEW *aView)
{
    m_view = aView;
}


bool SELECTION_TOOL::selectPoint(const VECTOR2I& aWhere, bool aOnDrag, bool* aSelectionCancelledFlag)
{
    ClearSelection();

    BOX2I selectionBox = m_area->GetBoundingBox();
    std::vector<VIEW::LAYER_ITEM_PAIR> candidates;
    m_view->Query(selectionBox, candidates);

    for (auto item : candidates)
        select(static_cast<BOARD_ITEM*>(item.first));

    return false;
}

void SELECTION_TOOL::select(KIGFX::BOARD_ITEM* aItem)
{
	highlight(aItem, SELECTED, &m_selection);
}

void SELECTION_TOOL::unselect(KIGFX::BOARD_ITEM* aItem)
{
    unhighlight(aItem, SELECTED, &m_selection);
}

void SELECTION_TOOL::highlight(KIGFX::BOARD_ITEM* aItem, int aHighlightMode, SELECTION* aGroup)
{
    if (aGroup)
        aGroup->Add(aItem);

	highlightInternal(aItem, aHighlightMode, aGroup != nullptr);

    m_view->Update(aItem, KIGFX::REPAINT);
}

void SELECTION_TOOL::unhighlight(KIGFX::BOARD_ITEM* aItem, int aHighlightMode, SELECTION* aGroup)
{
    if (aGroup)
		aGroup->Remove(aItem);

	unhighlightInternal(aItem, aHighlightMode, aGroup != nullptr);

    m_view->Update(aItem, KIGFX::REPAINT);
}

void SELECTION_TOOL::highlightInternal(KIGFX::BOARD_ITEM* aItem, int aHighlightMode, bool aUsingOverlay)
{
    if (aHighlightMode == SELECTED)
        aItem->SetSelected();
    else if (aHighlightMode == BRIGHTENED)
        aItem->SetBrightened();

    if (aUsingOverlay && aHighlightMode != BRIGHTENED)
        m_view->Hide(aItem, true);    // Hide the original item, so it is shown only on overlay
}

void SELECTION_TOOL::unhighlightInternal(KIGFX::BOARD_ITEM* aItem, int aHighlightMode, bool aUsingOverlay)
{
    if (aHighlightMode == SELECTED)
        aItem->ClearSelected();
    else if (aHighlightMode == BRIGHTENED)
        aItem->ClearBrightened();

    if (aUsingOverlay && aHighlightMode != BRIGHTENED)
    {
        m_view->Hide(aItem, false);   // Restore original item visibility...
        m_view->Update(aItem);        // ... and make sure it's redrawn un-selected
    }
}

