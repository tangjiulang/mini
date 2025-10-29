#pragma once

#include "gal/include/painter.hxx"
#include "data_render_settings.hxx"

namespace KIGFX {

class DATA_Triangle;
class DATA_Rectangle;
class DATA_Line;
class DATA_Circle;

class DATA_PAINTER : public PAINTER
{
public:
	DATA_PAINTER(GAL* aGal);

	virtual DATA_RENDER_SETTINGS* GetSettings() override
	{
		return &m_dataSettings;
	}
	virtual bool Draw(const VIEW_ITEM* aItem, int aLayer) override;
protected:
	void draw(const DATA_Triangle* aTriangle, int aLayer);
	void draw(const DATA_Rectangle* a_Rectangle, int aLayer);
	void draw(const DATA_Line* aLine, int aLayer);
	void draw(const DATA_Circle* aCircle, int aLayer);
protected:
	DATA_RENDER_SETTINGS m_dataSettings;
};

}