#pragma once

#include <gal/include/gal.hxx>
#include <view_item.hxx>
#include <deque>

namespace KIGFX
{
    /**
     * Extend #VIEW_ITEM by possibility of grouping items into a single object.
     *
     * VIEW_GROUP does not take over ownership of the held items. The main purpose of this class is
     * to group items and draw them on a single layer (in particular the overlay).
     */
    class VIEW_GROUP : public VIEW_ITEM
    {
    public:
        VIEW_GROUP(VIEW* aView = nullptr);
        virtual ~VIEW_GROUP();

        // We own at least one list of raw pointers.  Don't let the compiler fill in copy c'tors that
        // will only land us in trouble.
        VIEW_GROUP(const VIEW_GROUP&) = delete;
        VIEW_GROUP& operator=(const VIEW_GROUP&) = delete;

        std::string GetClass() const override;

        /**
         * Return the number of stored items.
         */
        virtual unsigned int GetSize() const;

        /**
         * Add an item to the group.
         */
        virtual void Add(VIEW_ITEM* aItem);

        /**
         * Remove an item from the group.
         */
        virtual void Remove(VIEW_ITEM* aItem);

        /**
         * Remove all the stored items from the group.
         */
        virtual void Clear();

        virtual VIEW_ITEM* GetItem(unsigned int aIdx) const;

        /**
         * Return the bounding box for all stored items covering all its layers.
         */
        virtual const BOX2I ViewBBox() const override;

        /**
         * Draw all the stored items in the group on the given layer.
         *
         * @param aLayer is the layer which should be drawn.
         * @param aView is the VIEW that should be used for drawing.
         */
        virtual void ViewDraw(int aLayer, VIEW* aView) const override;

        ///@copydoc VIEW_ITEM::ViewGetLayers
        std::vector<int> ViewGetLayers() const override;

        /**
         * Set layer used to draw the group.
         */
        inline virtual void SetLayer(int aLayer)
        {
            m_layer = aLayer;
        }

        /**
         * Free all the items that were added to the group.
         */
        void FreeItems();

    protected:
        virtual const std::vector<VIEW_ITEM*> updateDrawList() const;

    protected:
        int                     m_layer;
        std::vector<VIEW_ITEM*> m_groupItems;       // No ownership.
    };

} // namespace KIGFX
