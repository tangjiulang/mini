#include <set>
#include <kicad_algo.hxx>
#include <algorithm>
#include <view_group.hxx>
#include <view.hxx>
#include <view_item.hxx>
#include <gal/include/painter.hxx>
#include <gal/include/graphics_abstraction_layer.hxx>
#include <layer_ids.hxx>

using namespace KIGFX;

VIEW_GROUP::VIEW_GROUP(VIEW* aView) :
    VIEW_ITEM(),
    m_layer(LAYER_SELECT_OVERLAY)
{
}


VIEW_GROUP::~VIEW_GROUP()
{
    // VIEW_ITEM destructor removes the object from its parent view
}


std::string VIEW_GROUP::GetClass() const
{
    return std::string("VIEW_GROUP");
}


void VIEW_GROUP::Add(VIEW_ITEM* aItem)
{
    m_groupItems.push_back(aItem);
}


void VIEW_GROUP::Remove(VIEW_ITEM* aItem)
{
    std::erase(m_groupItems, aItem);
}


void VIEW_GROUP::Clear()
{
    m_groupItems.clear();
}


unsigned int VIEW_GROUP::GetSize() const
{
    return m_groupItems.size();
}


VIEW_ITEM* VIEW_GROUP::GetItem(unsigned int idx) const
{
    return m_groupItems[idx];
}


const BOX2I VIEW_GROUP::ViewBBox() const
{
    BOX2I bb;

    if (!m_groupItems.size())
    {
        bb.SetMaximum();
    }
    else
    {
        bb = m_groupItems[0]->ViewBBox();

        for (VIEW_ITEM* item : m_groupItems)
            bb.Merge(item->ViewBBox());
    }

    return bb;
}


void VIEW_GROUP::ViewDraw(int aLayer, VIEW* aView) const
{
    KIGFX::GAL* gal = aView->GetGAL();
    PAINTER* painter = aView->GetPainter();
    bool        isSelection = m_layer == LAYER_SELECT_OVERLAY;

    const std::vector<VIEW_ITEM*> drawList = updateDrawList();

    std::map<int, std::vector<VIEW_ITEM*>> layer_item_map;

    // Build a list of layers used by the items in the group
    for (VIEW_ITEM* item : drawList)
    {
        if (aView->IsHiddenOnOverlay(item))
            continue;

        std::vector<int> layers = item->ViewGetLayers();

        for (auto layer : layers)
        {
            if (layer > LAYER_ID_COUNT)
                continue;
            layer_item_map[layer].push_back(item);
        }
    }

    if (layer_item_map.empty())
        return;

    std::vector<int> layers;
    layers.reserve(layer_item_map.size());

    for (const std::pair<const int, std::vector<VIEW_ITEM*>>& entry : layer_item_map)
        layers.push_back(entry.first);

    aView->SortLayers(layers);

    // Now draw the layers in sorted order

    GAL_SCOPED_ATTRS scopedAttrs(*gal, GAL_SCOPED_ATTRS::LAYER_DEPTH);

    for (int layer : layers)
    {
        bool draw = aView->IsLayerVisible(layer);

        if (IsZoneFillLayer(layer))
        {
            // The visibility of solid areas must follow the visiblility of the zone layer
            int zone_main_layer = layer - LAYER_ZONE_START;
            draw = aView->IsLayerVisible(zone_main_layer);
        }
        else if (IsPadCopperLayer(layer))
        {
            draw = aView->IsLayerVisible(layer - LAYER_PAD_COPPER_START);
        }
        else if (IsViaCopperLayer(layer))
        {
            draw = aView->IsLayerVisible(layer - LAYER_VIA_COPPER_START);
        }
        else if (IsClearanceLayer(layer))
        {
            draw = aView->IsLayerVisible(layer - LAYER_CLEARANCE_START);
        }
        else if (IsPointsLayer(layer))
        {
            draw = aView->IsLayerVisible(layer - LAYER_POINT_START);
        }

        if (isSelection)
        {
            switch (layer)
            {
            case LAYER_PAD_PLATEDHOLES:
            case LAYER_PAD_HOLEWALLS:
                draw = true;
                break;

            default:
                break;
            }
        }

        if (draw)
        {
            gal->AdvanceDepth();

            for (VIEW_ITEM* item : layer_item_map[layer])
            {
                // Ignore LOD scale for selected items, but don't ignore things explicitly
                // hidden.
                if (item->ViewGetLOD(layer, aView) == LOD_HIDE)
                    continue;

                if (!painter->Draw(item, layer))
                    item->ViewDraw(layer, aView); // Alternative drawing method
            }
        }
    }
}


std::vector<int> VIEW_GROUP::ViewGetLayers() const
{
    // Everything is displayed on a single layer
    return { m_layer };
}


void VIEW_GROUP::FreeItems()
{
    for (unsigned int i = 0; i < GetSize(); i++)
        delete GetItem(i);

    Clear();
}


const std::vector<VIEW_ITEM*> VIEW_GROUP::updateDrawList() const
{
    return m_groupItems;
}
