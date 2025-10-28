#pragma once

#include "gal/include/opengl_gal.hxx"
#include "render_settings.hxx"

namespace KIGFX 
{
class VIEW_ITEM;
class PAINTER {
public:
    PAINTER(KIGFX::GAL* gal);
    virtual ~PAINTER();
    void DrawLine(const VECTOR2D& aStartPoint, const VECTOR2D& aEndPoint);

    /**
        * Return a pointer to current settings that are going to be used when drawing items.
        *
        * @return Current rendering settings.
        */
    virtual RENDER_SETTINGS* GetSettings() = 0;

    /**
        * Takes an instance of VIEW_ITEM and passes it to a function that knows how to draw
        * the item.
        *
        * @param aItem is an item to be drawn.
        * @param aLayer tells which layer is currently rendered so that draw functions may
        *               know what to draw (eg. for pads there are separate layers for holes,
        *               because they have other dimensions then the pad itself.
        */
    virtual bool Draw(const VIEW_ITEM* aItem, int aLayer) = 0;

protected:
    KIGFX::GAL* m_gal;
};
}

