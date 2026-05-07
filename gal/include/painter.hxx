#pragma once

#include "gal/include/opengl_gal.hxx"
#include "gal/include/insert_vertex.hxx"
#include "render_settings.hxx"

namespace MINI 
{
class VIEW;
class VIEW_ITEM;
class PAINTER {
public:
    PAINTER(MINI::GAL* gal, bool isCached = false);
    virtual ~PAINTER();

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

    /**
     * Changes Graphics Abstraction Layer used for drawing items for a new one.
     *
     * @param aGal is the new GAL instance.
     */
    void SetGAL(GAL* aGal)
    {
        m_gal = aGal;
    }

    void SetView(const VIEW* aView)
    {
        m_view = aView;
    }

protected:
    GAL* m_gal;
    const VIEW* m_view;
public:
	INSERT_VERTEX* m_insertVertex;
};
}

