#pragma once

#include "view_item.hxx"
#include "layer_ids.hxx"

namespace KIGFX {

enum ITEM_TYPE {
	NOT_USED = -1,
	TYPE_NOT_INIT = 0,
	TRIANGLE,
	RECTANGLE,
	LINE,
	CIRCLE
};

	
class BOARD_ITEM : public VIEW_ITEM {
public:
	BOARD_ITEM(ITEM_TYPE type = ITEM_TYPE::NOT_USED) 
		: VIEW_ITEM(false, true),  
		m_structType(type), 
		m_layer(F_Cu) {}
	inline ITEM_TYPE Type() const { return m_structType; }
	virtual const BOX2I ViewBBox() const override;
	virtual const BOX2I GetBoundingBox() const;

	virtual std::vector<int> ViewGetLayers() const override;

protected:
	ITEM_TYPE        m_structType;
	PCB_LAYER_ID     m_layer;
};
}