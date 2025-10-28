#pragma once

#include <bitset>
#include <vector>
#include <limits>
#include <numeric>

#include <gal/include/gal.hxx>
#include <box2.hxx>
#include "view_data.hxx"

namespace KIGFX
{
    // Forward declarations
    class VIEW;


    /**
     * An abstract base class for deriving all objects that can be added to a VIEW.
     *
     * Its role is to:
     * - communicate geometry, appearance and visibility updates to the associated dynamic VIEW,
     * - provide a bounding box for redraw area calculation,
     * - (optional) draw the object using the #GAL API functions for #PAINTER-less implementations.
     *
     * VIEW_ITEM objects are never owned by a #VIEW. A single VIEW_ITEM can belong to any number of
     * static VIEWs, but only one dynamic VIEW due to storage of only one VIEW reference.
     */
    class VIEW_ITEM
    {
    public:
        VIEW_ITEM(bool isSCH_ITEM = false, bool isBOARD_ITEM = false) :
            m_isSCH_ITEM(isSCH_ITEM),
            m_isBOARD_ITEM(isBOARD_ITEM),
            m_viewPrivData(nullptr),
            m_forcedTransparency(0.0)
        {
        }

        virtual ~VIEW_ITEM();

        VIEW_ITEM(const VIEW_ITEM& aOther) = delete;
        VIEW_ITEM& operator=(const VIEW_ITEM& aOther) = delete;

        bool IsSCH_ITEM() const { return m_isSCH_ITEM; }
        bool IsBOARD_ITEM() const { return m_isBOARD_ITEM; }

        /**
         * Return the class name.
         */
        virtual std::string GetClass() const = 0;

        /**
         * Return the bounding box of the item covering all its layers.
         *
         * @return the current bounding box.
         */
        virtual const BOX2I ViewBBox() const = 0;

        /**
         * Draw the parts of the object belonging to layer aLayer.
         *
         * An alternative way for drawing objects if there is no #PAINTER assigned for the view
         * or if the PAINTER doesn't know how to paint this particular implementation of VIEW_ITEM.
         * The preferred way of drawing is to design an appropriate PAINTER object, the method
         * below is intended only for quick hacks and debugging purposes.
         *
         * @param aLayer is the current drawing layer.
         * @param aView is a pointer to the #VIEW device we are drawing on.
         */
        virtual void ViewDraw(int aLayer, VIEW* aView) const
        {
        }

        /**
         * Return the all the layers within the VIEW the object is painted on.
         *
         * For instance, a #PAD spans zero or more copper layers and a few technical layers.
         * ViewDraw() or PAINTER::Draw() is repeatedly called for each of the layers returned
         * by ViewGetLayers(), depending on the rendering order.
         */
        virtual std::vector<int> ViewGetLayers() const = 0;

        /**
         * Return the level of detail (LOD) of the item.
         *
         * A level of detail is the minimal #VIEW scale that is sufficient for an item to be shown
         * on a given layer.
         *
         * Use @ref LOD_HIDE and @ref LOD_SHOW constants to hide or show the item unconditionally.
         *
         * Use @ref lodScaleForThreshold() to calculate the LOD scale for when the item
         * passes a certain threshold size on screen.
         *
         * @param aLayer is the current drawing layer.
         * @param aView is a pointer to the #VIEW device we are drawing on.
         * @return the level of detail. 0 always shows the item, because the actual zoom level
         *         (or VIEW scale) is always > 0
         */
        virtual double ViewGetLOD(int aLayer, const VIEW* aView) const
        {
            // By default always show the item
            return LOD_SHOW;
        }

        VIEW_ITEM_DATA* viewPrivData() const
        {
            return m_viewPrivData;
        }

        void SetForcedTransparency(double aForcedTransparency)
        {
            m_forcedTransparency = aForcedTransparency;
        }

        double GetForcedTransparency() const
        {
            return m_forcedTransparency;
        }

    protected:
        /**
         * Return this constant from ViewGetLOD() to hide the item unconditionally.
         */
        static constexpr double LOD_HIDE = std::numeric_limits<double>::max();

        /**
         * Return this constant from ViewGetLOD() to show the item unconditionally.
         */
        static constexpr double LOD_SHOW = 0.0;

        /**
         * Get the scale at which aWhatIu would be drawn at the same size as
         * aThresholdIu on screen.
         *
         * This is useful when a level-of-detail is defined in terms of a threshold
         * size (i.e. 'only draw X when it will be bigger than Y size on screen').
         *
         * E.g. if aWhatIu is 1000 and aThresholdIu is 100, then the item will be
         * the same size as the threshold at 0.1 scale. Returning that 0.1 as the LoD
         * will hide the item when the scale is less than 0.1 - i.e. smaller than the
         * threshold.
         *
         * Because even at zoom 1.0, 1mm in KiCad may not be exactly 1mm on a physical
         * screen, the threshold may not be exact in practice.
         */
        static double lodScaleForThreshold(const KIGFX::VIEW* aView, int aWhatIu, int aThresholdIu);

    private:
        friend class VIEW;

        bool            m_isSCH_ITEM;
        bool            m_isBOARD_ITEM;
        VIEW_ITEM_DATA* m_viewPrivData;
        double          m_forcedTransparency;  ///< Additional transparency for diff'ing items.
    };

} // namespace KIGFX

