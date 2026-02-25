#pragma once

#include "selection.hxx"
#include "selection_area.hxx"

class SELECTION_TOOL {
public:
    SELECTION_TOOL();
    ~SELECTION_TOOL();


	SELECTION& GetSelection();

	void ClearSelection(bool aQuietMode = false);

    // update select area
    void SetOrigin(const VECTOR2I& aOrigin);
	void SetEnd(const VECTOR2I& aEnd);

	int SelectRectArea();

    void SetView(MINI::VIEW *aView);

    /**
     * Select an item pointed by the parameter \a aWhere.
     *
     * If there is more than one item at that place, there is a menu displayed that allows
     * one to choose the item.
     *
     * @param aWhere is the place where the item should be selected.
     * @param aOnDrag indicates whether a drag operation is being performed.
     * @param aSelectionCancelledFlag allows the function to inform its caller that a selection
     *                                was canceled (for instance, by clicking outside of the
     *                                disambiguation menu).
     * @param aClientFilter a callback to allow tool- or action-specific filtering.
     * @return whether or not the selection is empty.
     */
    bool selectPoint(const VECTOR2I& aWhere, bool aOnDrag = false,
                     bool* aSelectionCancelledFlag = nullptr);

    /**
     * Select an item under the cursor unless there is something already selected.
     *
     * @param aForceSelect [optional] Forces an item to be selected even if there is already a
     *                     selection.
     * @param aClientFilter A callback to allow tool- or action-specific filtering.
     * @return whether or not the selection is empty.
     */
    bool selectCursor(bool aForceSelect = false);

    void select(MINI::BOARD_ITEM* aItem);
    void unselect(MINI::BOARD_ITEM* aItem);
    void highlight(MINI::BOARD_ITEM* aItem, int aHighlightMode, SELECTION* aGroup = nullptr);
    void unhighlight(MINI::BOARD_ITEM* aItem, int aHighlightMode, SELECTION* aGroup = nullptr);

private:
    void highlightInternal(MINI::BOARD_ITEM* aItem, int aHighlightMode, bool aUsingOverlay);

    void unhighlightInternal(MINI::BOARD_ITEM* aItem, int aHighlightMode, bool aUsingOverlay);

private:
    SELECTION            m_selection;            // Current state of selection
    MINI::VIEW*         m_view;
    MINI::SELECTION_AREA *m_area;
};