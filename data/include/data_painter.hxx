#pragma once

#include "gal/include/painter.hxx"
#include "data_render_settings.hxx"
#include <text_attributes.hxx>
#include <font_metrics.hxx>

namespace MINI
{

class DATA_Triangle;
class DATA_Rectangle;
class DATA_Line;
class DATA_LineBatch;
class DATA_Circle;
class DATA_Polyline;
class DATA_Polygon;
class DATA_Arc;
class DATA_PolySet;
class DATA_Text;

class DATA_PAINTER : public PAINTER
{
public:
    DATA_PAINTER(GAL* aGal);

    virtual DATA_RENDER_SETTINGS* GetSettings() override { return &m_dataSettings; }
    virtual bool                  Draw(const VIEW_ITEM* aItem, int aLayer) override;

protected:
    void draw(const DATA_Triangle* aTriangle, int aLayer);
    void draw(const DATA_Rectangle* a_Rectangle, int aLayer);
    void draw(const DATA_Line* aLine, int aLayer);
    void draw(const DATA_LineBatch* aLineBatch, int aLayer);
    void draw(const DATA_Circle* aCircle, int aLayer);
    void draw(const DATA_Polyline* aPolygon, int aLayer);
    void draw(const DATA_Polygon* aPolygon, int aLayer);
    void draw(const DATA_Arc* aArc, int aLayer);
    void draw(DATA_PolySet* aPolySet, int aLayer);
    void draw(const DATA_Text* aText, int aLayer);

private:
    void strokeText(const std::string& aText, const VECTOR2I& aPosition, const TEXT_ATTRIBUTES& aAttrs,
                    const KIFONT::METRICS& aFontMetrics);

protected:
    DATA_RENDER_SETTINGS m_dataSettings;
};

} // namespace MINI
