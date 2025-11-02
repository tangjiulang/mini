#pragma once
#include <vector>
#include <unordered_map>
#include <spdlog/spdlog.h>

#include "view_def.hxx"
#include "box2.hxx"

namespace KIGFX {

class VIEW;

class VIEW_ITEM_DATA
{
public:
    VIEW_ITEM_DATA() :
        m_view(nullptr),
        m_flags(KIGFX::VISIBLE),
        m_requiredUpdate(KIGFX::NONE),
        m_drawPriority(0),
        m_cachedIndex(-1),
        m_groups(nullptr),
        m_groupsSize(0) {
    }

    ~VIEW_ITEM_DATA()
    {
        deleteGroups();
    }

    int GetFlags() const
    {
        return m_flags;
    }

private:
    friend class VIEW;

    /**
        * Return number of the group id for the given layer, or -1 in case it was not cached before.
        *
        * @param aLayer is the layer number for which group id is queried.
        * @return group id or -1 in case there is no group id (ie. item is not cached).
        */
    int getGroup(int aLayer) const
    {
        for (int i = 0; i < m_groupsSize; ++i)
        {
            if (m_groups[i].first == aLayer)
                return m_groups[i].second;
        }

        return -1;
    }

    /**
        * Set a group id for the item and the layer combination.
        *
        * @param aLayer is the layer number.
        * @param aGroup is the group id.
        */
    void setGroup(int aLayer, int aGroup)
    {
        // Look if there is already an entry for the layer
        for (int i = 0; i < m_groupsSize; ++i)
        {
            if (m_groups[i].first == aLayer)
            {
                m_groups[i].second = aGroup;
                return;
            }
        }

        // If there was no entry for the given layer - create one
        std::pair<int, int>* newGroups = new std::pair<int, int>[m_groupsSize + 1];

        if (m_groupsSize > 0)
        {
            std::copy(m_groups, m_groups + m_groupsSize, newGroups);
            delete[] m_groups;
        }

        m_groups = newGroups;
        newGroups[m_groupsSize++] = { aLayer, aGroup };
    }


    /**
        * Remove all of the stored group ids. Forces recaching of the item.
        */
    void deleteGroups()
    {
        //delete[] m_groups;
        m_groups = nullptr;
        m_groupsSize = 0;
    }

    /**
        * Return information if the item uses at least one group id (ie. if it is cached at all).
        *
        * @returns true in case it is cached at least for one layer.
        */
    inline bool storesGroups() const
    {
        return m_groupsSize > 0;
    }

    /**
        * Reorder the stored groups (to facilitate reordering of layers).
        *
        * @see VIEW::ReorderLayerData
        *
        * @param aReorderMap is the mapping of old to new layer ids
        */
    void reorderGroups(std::unordered_map<int, int> aReorderMap)
    {
        for (int i = 0; i < m_groupsSize; ++i)
        {
            int orig_layer = m_groups[i].first;
            int new_layer = orig_layer;

            if (aReorderMap.count(orig_layer))
                new_layer = aReorderMap.at(orig_layer);

            m_groups[i].first = new_layer;
        }
    }

    /**
        * Save layers used by the item.
        *
        * @param aLayers is an array containing layer numbers to be saved.
        * @param aCount is the size of the array.
        */
    void saveLayers(const std::vector<int>& aLayers)
    {
        m_layers.clear();

        for (int layer : aLayers)
        {
            if (layer >= 0 && layer < 2048) continue;
            else {
                spdlog::warn(std::format("Invalid layer number: {}", layer));
            }
            m_layers.push_back(layer);
        }
    }

    /**
        * Return current update flag for an item.
        */
    int requiredUpdate() const
    {
        return m_requiredUpdate;
    }

    /**
        * Mark an item as already updated, so it is not going to be redrawn.
        */
    void clearUpdateFlags()
    {
        m_requiredUpdate = NONE;
    }

    /**
        * Return if the item should be drawn or not.
        */
    bool isRenderable() const
    {
        return m_flags == VISIBLE;
    }

    VIEW* m_view;             ///< Current dynamic view the item is assigned to.
    int                  m_flags;            ///< Visibility flags
    int                  m_requiredUpdate;   ///< Flag required for updating
    int                  m_drawPriority;     ///< Order to draw this item in a layer, lowest first
    int                  m_cachedIndex;      ///< Cached index in m_allItems.

    std::pair<int, int>* m_groups;           ///< layer_number:group_id pairs for each layer the
    ///< item occupies.
    int                  m_groupsSize;

    std::vector<int>     m_layers;           /// Stores layer numbers used by the item.

    BOX2I                m_bbox;             /// Cached inserted Bbox for faster removals.
};
}