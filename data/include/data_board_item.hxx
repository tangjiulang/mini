#pragma once

#include "view_item.hxx"
#include "data_item_flags.hxx"
#include "layer_ids.hxx"

namespace KIGFX {

enum class ITEM_TYPE {
	NOT_USED = -1,
	TYPE_NOT_INIT = 0,
	TRIANGLE,
	RECTANGLE,
	LINE,
    ARC,
	CIRCLE,
	POLYGON,
    POLYLINE,
	GROUP
};

using ITEM_FLAGS = uint32_t;

class BOARD_ITEM : public VIEW_ITEM {
public:
	BOARD_ITEM(ITEM_TYPE type = ITEM_TYPE::NOT_USED, PCB_LAYER_ID layer = F_Cu)
        : VIEW_ITEM(false, true)
    {
		m_structType = type;
		m_layer = layer;
        m_flags = 0;
	}
	inline ITEM_TYPE Type() const { return m_structType; }
	virtual const BOX2I ViewBBox() const override;
	virtual const BOX2I GetBoundingBox() const;

	virtual std::vector<int> ViewGetLayers() const override;

    inline bool IsModified() const { return m_flags & IS_CHANGED; }
    inline bool IsNew() const { return m_flags & IS_NEW; }
    inline bool IsMoving() const { return m_flags & IS_MOVING; }

    inline bool IsSelected() const { return m_flags & SELECTED; }
    inline bool IsEntered() const { return m_flags & ENTERED; }
    inline bool IsBrightened() const { return m_flags & BRIGHTENED; }


    inline void SetSelected() { SetFlags(SELECTED); }
    inline void SetBrightened() { SetFlags(BRIGHTENED); }

    inline void ClearSelected() { ClearFlags(SELECTED); }
    inline void ClearBrightened() { ClearFlags(BRIGHTENED); }

    void SetModified();

    void           SetFlags(ITEM_FLAGS aMask) { m_flags |= aMask; }
    void           XorFlags(ITEM_FLAGS aMask) { m_flags ^= aMask; }
    void           ClearFlags(ITEM_FLAGS aMask = ITEM_ALL_FLAGS) { m_flags &= ~aMask; }
    ITEM_FLAGS GetFlags() const { return m_flags; }
    bool           HasFlag(ITEM_FLAGS aFlag) const { return (m_flags & aFlag) == aFlag; }

    ITEM_FLAGS GetEditFlags() const
    {
        constexpr int mask =
            (IS_NEW | IS_PASTED | IS_MOVING | IS_BROKEN | IS_CHANGED | STRUCT_DELETED);

        return m_flags & mask;
    }

    virtual void ClearEditFlags()
    {
        ClearFlags(GetEditFlags());
    }

    ITEM_FLAGS GetTempFlags() const
    {
        constexpr int mask = (CANDIDATE | SELECTED_BY_DRAG | IS_LINKED | SKIP_STRUCT | SELECTION_CANDIDATE
                              | CONNECTIVITY_CANDIDATE);

        return m_flags & mask;
    }

    virtual void ClearTempFlags()
    {
        ClearFlags(GetTempFlags());
    }

protected:
    ITEM_FLAGS		 m_flags;
	ITEM_TYPE        m_structType;
	PCB_LAYER_ID     m_layer;
};
}