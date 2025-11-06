#include <layer_ids.hxx>
#include <trace_helpers.hxx>

#include <view.hxx>
//#include <view/view_group.h>
#include <view_item.hxx>
#include <view_data.hxx>
#include <view_rtree.hxx>
//#include <view/view_overlay.h>

#include <gal/include/painter.hxx>
#include <shape_poly_set.hxx>
#include <gal/include/definitions.hxx>
#include <gal/include/graphics_abstraction_layer.hxx>
#include <algorithm>

#include <profile.hxx>

namespace KIGFX {

    class VIEW;


    void VIEW::OnDestroy(VIEW_ITEM* aItem)
    {
        if (aItem->m_viewPrivData)
        {
            if (aItem->m_viewPrivData->m_view)
                aItem->m_viewPrivData->m_view->VIEW::Remove(aItem);

            delete aItem->m_viewPrivData;
            aItem->m_viewPrivData = nullptr;
        }
    }


    VIEW::VIEW() :
        m_enableOrderModifier(true),
        m_scale(4.0),
        m_minScale(0.2), m_maxScale(50000.0),
        m_mirrorX(false), m_mirrorY(false),
        m_painter(nullptr),
        m_gal(nullptr),
        m_useDrawPriority(false),
        m_nextDrawPriority(0),
        m_reverseDrawOrder(false)
    {
        // Set m_boundary to define the max area size. The default area size
        // is defined here as the max value of a int.
        // this is a default value acceptable for Pcbnew and Gerbview, but too large for Eeschema.
        // So in eeschema a call to SetBoundary() with a smaller value will be needed.
        typedef std::numeric_limits<int> coord_limits;
        double pos = coord_limits::lowest() / 2 + coord_limits::epsilon();
        double size = coord_limits::max() - coord_limits::epsilon();
        m_boundary.SetOrigin(pos, pos);
        m_boundary.SetSize(size, size);

        m_allItems.reset(new std::vector<VIEW_ITEM*>);
        m_allItems->reserve(32768);

        // Redraw everything at the beginning
        MarkDirty();

        // View uses layers to display EDA_ITEMs (item may be displayed on several layers, for example
        // pad may be shown on pad, pad hole and solder paste layers). There are usual copper layers
        // (eg. F.Cu, B.Cu, internal and so on) and layers for displaying objects such as texts,
        // silkscreen, pads, vias, etc.
        for (int ii = 0; ii < VIEW_MAX_LAYERS; ++ii)
        {
            auto [it, _] = m_layers.emplace(ii, VIEW_LAYER());
            VIEW_LAYER& l = it->second;

            l.items = std::make_shared<VIEW_RTREE>();
            l.id = ii;
            l.renderingOrder = ii;
            l.visible = true;
            l.displayOnly = false;
            l.diffLayer = false;
            l.hasNegatives = false;
            l.target = TARGET_CACHED;
        }

        sortOrderedLayers();

        //m_preview.reset(new KIGFX::VIEW_GROUP());
        //Add(m_preview.get());
    }


    VIEW::~VIEW()
    {
        //Remove(m_preview.get());
    }


    void VIEW::Add(VIEW_ITEM* aItem, int aDrawPriority)
    {
        if (aDrawPriority < 0)
            aDrawPriority = m_nextDrawPriority++;

        if (!aItem->m_viewPrivData)
            aItem->m_viewPrivData = new VIEW_ITEM_DATA;

        //wxASSERT_MSG(aItem->m_viewPrivData->m_view == nullptr || aItem->m_viewPrivData->m_view == this,
        //    wxS("Already in a different view!"));

        aItem->m_viewPrivData->m_view = this;
        aItem->m_viewPrivData->m_drawPriority = aDrawPriority;
        const BOX2I bbox = aItem->ViewBBox();
        aItem->m_viewPrivData->m_bbox = bbox;
        aItem->m_viewPrivData->m_cachedIndex = m_allItems->size();

        std::vector<int> layers = aItem->ViewGetLayers();

        std::erase_if(layers, [](int layer)
            {
                return layer < 0 || layer >= VIEW_MAX_LAYERS;
            });

        if (layers.empty())
            return;

        aItem->viewPrivData()->saveLayers(layers);

        m_allItems->push_back(aItem);

        for (int layer : layers)
        {
            VIEW_LAYER& l = m_layers[layer];
            l.items->Insert(aItem, bbox);
            MarkTargetDirty(l.target);
        }

        SetVisible(aItem, true);
        Update(aItem, KIGFX::INITIAL_ADD);
    }


    void VIEW::Remove(VIEW_ITEM* aItem)
    {
        static int s_gcCounter = 0;

        if (aItem && aItem->m_viewPrivData)
        {
            if (aItem->m_viewPrivData->m_view == this) return;
            else aItem->GetClass();

            std::vector<VIEW_ITEM*>::iterator item = m_allItems->end();
            int                               cachedIndex = aItem->m_viewPrivData->m_cachedIndex;

            if (cachedIndex >= 0
                && cachedIndex < static_cast<size_t>(m_allItems->size())
                && (*m_allItems)[cachedIndex] == aItem)
            {
                item = m_allItems->begin() + cachedIndex;
            }
            else
            {
                item = std::find(m_allItems->begin(), m_allItems->end(), aItem);
            }

            if (item != m_allItems->end())
            {
                *item = nullptr;
                aItem->m_viewPrivData->clearUpdateFlags();

                s_gcCounter++;

                if (s_gcCounter > 4096)
                {
                    // Perform defragmentation
                    std::erase_if(*m_allItems,
                        [](VIEW_ITEM* it)
                        {
                            return it == nullptr;
                        });

                    // Update cached indices
                    for (size_t idx = 0; idx < m_allItems->size(); idx++)
                        (*m_allItems)[idx]->m_viewPrivData->m_cachedIndex = idx;

                    s_gcCounter = 0;
                }
            }

            const BOX2I* bbox = &aItem->m_viewPrivData->m_bbox;

            for (int layer : aItem->m_viewPrivData->m_layers)
            {
                VIEW_LAYER& l = m_layers[layer];
                l.items->Remove(aItem, bbox);
                MarkTargetDirty(l.target);

                // Clear the GAL cache
                int prevGroup = aItem->m_viewPrivData->getGroup(layer);

                if (prevGroup >= 0)
                    m_gal->DeleteGroup(prevGroup);
            }

            aItem->m_viewPrivData->deleteGroups();
            aItem->m_viewPrivData->m_view = nullptr;
        }
    }


    void VIEW::SetRequired(int aLayerId, int aRequiredId, bool aRequired)
    {
        if ((unsigned)aLayerId >= m_layers.size()) return;
        if ((unsigned)aRequiredId >= m_layers.size()) return;

        if (aRequired)
            m_layers[aLayerId].requiredLayers.insert(aRequiredId);
        else
            m_layers[aLayerId].requiredLayers.erase(aRequired);
    }


    int VIEW::Query(const BOX2I& aRect, std::vector<LAYER_ITEM_PAIR>& aResult) const
    {
        if (m_orderedLayers.empty())
            return 0;

        int  layer = UNDEFINED_LAYER;
        auto visitor =
            [&](VIEW_ITEM* item) -> bool
            {
                aResult.push_back(VIEW::LAYER_ITEM_PAIR(item, layer));
                return true;
            };

        std::vector<VIEW_LAYER*>::const_reverse_iterator i;

        // execute queries in reverse direction, so that items that are on the top of
        // the rendering stack are returned first.
        for (i = m_orderedLayers.rbegin(); i != m_orderedLayers.rend(); ++i)
        {
            // ignore layers that do not contain actual items (i.e. the selection box, menus, floats)
            if ((*i)->displayOnly || !(*i)->visible)
                continue;

            layer = (*i)->id;
            (*i)->items->Query(aRect, visitor);
        }

        return aResult.size();
    }


    void VIEW::Query(const BOX2I& aRect, const std::function<bool(VIEW_ITEM*)>& aFunc) const
    {
        if (m_orderedLayers.empty())
            return;

        for (const auto& i : m_orderedLayers)
        {
            // ignore layers that do not contain actual items (i.e. the selection box, menus, floats)
            if (i->displayOnly || !i->visible)
                continue;

            i->items->Query(aRect, aFunc);
        }
    }


    VECTOR2D VIEW::ToWorld(const VECTOR2D& aCoord, bool aAbsolute) const
    {
        const MATRIX3x3D& matrix = m_gal->GetScreenWorldMatrix();

        if (aAbsolute)
            return VECTOR2D(matrix * aCoord);
        else
            return VECTOR2D(matrix.GetScale().x * aCoord.x, matrix.GetScale().y * aCoord.y);
    }


    double VIEW::ToWorld(double aSize) const
    {
        const MATRIX3x3D& matrix = m_gal->GetScreenWorldMatrix();

        return fabs(matrix.GetScale().x * aSize);
    }


    VECTOR2D VIEW::ToScreen(const VECTOR2D& aCoord, bool aAbsolute) const
    {
        const MATRIX3x3D& matrix = m_gal->GetWorldScreenMatrix();

        if (aAbsolute)
            return VECTOR2D(matrix * aCoord);
        else
            return VECTOR2D(matrix.GetScale().x * aCoord.x, matrix.GetScale().y * aCoord.y);
    }


    double VIEW::ToScreen(double aSize) const
    {
        const MATRIX3x3D& matrix = m_gal->GetWorldScreenMatrix();

        return matrix.GetScale().x * aSize;
    }


    void VIEW::CopySettings(const VIEW* aOtherView)
    {
        //wxASSERT_MSG(false, wxT("This is not implemented"));
    }


    void VIEW::SetGAL(GAL* aGal)
    {
        bool recacheGroups = (m_gal != nullptr);    // recache groups only if GAL is reassigned
        m_gal = aGal;

        // clear group numbers, so everything is going to be recached
        if (recacheGroups)
            clearGroupCache();

        // every target has to be refreshed
        MarkDirty();

        // force the new GAL to display the current viewport.
        SetCenter(m_center);
        SetScale(m_scale);
        SetMirror(m_mirrorX, m_mirrorY);
    }


    BOX2D VIEW::GetViewport() const
    {
        BOX2D    rect;
        VECTOR2D screenSize = m_gal->GetScreenPixelSize();

        rect.SetOrigin(ToWorld(VECTOR2D(0, 0)));
        rect.SetEnd(ToWorld(screenSize));

        return rect.Normalize();
    }


    void VIEW::SetViewport(const BOX2D& aViewport)
    {
        VECTOR2D ssize = ToWorld(m_gal->GetScreenPixelSize(), false);

        if (!(fabs(ssize.x) > 0 && fabs(ssize.y) > 0)) return;

        VECTOR2D centre = aViewport.Centre();
        VECTOR2D vsize = aViewport.GetSize();
        double   zoom = 1.0 / std::max(fabs(vsize.x / ssize.x), fabs(vsize.y / ssize.y));

        SetCenter(centre);
        SetScale(GetScale() * zoom);
    }


    void VIEW::SetMirror(bool aMirrorX, bool aMirrorY)
    {
        //wxASSERT_MSG(!aMirrorY, _("Mirroring for Y axis is not supported yet"));
        

        m_mirrorX = aMirrorX;
        m_mirrorY = aMirrorY;
        m_gal->SetFlip(aMirrorX, aMirrorY);

        // Redraw everything
        MarkDirty();
    }


    void VIEW::SetScale(double aScale, VECTOR2D aAnchor)
    {
        if (aAnchor == VECTOR2D(0, 0))
            aAnchor = m_center;

        VECTOR2D a = ToScreen(aAnchor);

        if (aScale < m_minScale)
            m_scale = m_minScale;
        else if (aScale > m_maxScale)
            m_scale = m_maxScale;
        else
            m_scale = aScale;

        m_gal->SetZoomFactor(m_scale);
        m_gal->ComputeWorldScreenMatrix();

        VECTOR2D delta = ToWorld(a) - aAnchor;

        SetCenter(m_center - delta);

        // Redraw everything after the viewport has changed
        MarkDirty();
    }


    void VIEW::SetCenter(const VECTOR2D& aCenter)
    {
        m_center = aCenter;

        if (!m_boundary.Contains(aCenter))
        {
            if (m_center.x < m_boundary.GetLeft())
                m_center.x = m_boundary.GetLeft();
            else if (aCenter.x > m_boundary.GetRight())
                m_center.x = m_boundary.GetRight();

            if (m_center.y < m_boundary.GetTop())
                m_center.y = m_boundary.GetTop();
            else if (m_center.y > m_boundary.GetBottom())
                m_center.y = m_boundary.GetBottom();
        }

        m_gal->SetLookAtPoint(m_center);
        m_gal->ComputeWorldScreenMatrix();

        // Redraw everything after the viewport has changed
        MarkDirty();
    }


    void VIEW::SetCenter(const VECTOR2D& aCenter, const std::vector<BOX2D>& obscuringScreenRects)
    {
        if (obscuringScreenRects.empty())
            return SetCenter(aCenter);

        BOX2D          screenRect({ 0, 0 }, m_gal->GetScreenPixelSize());
        SHAPE_POLY_SET unobscuredPoly(screenRect);
        VECTOR2D       unobscuredCenter = screenRect.Centre();

        for (const BOX2D& obscuringScreenRect : obscuringScreenRects)
        {
            SHAPE_POLY_SET obscuringPoly(obscuringScreenRect);
            unobscuredPoly.BooleanSubtract(obscuringPoly);
        }

        /*
         * Perform a step-wise deflate to find the center of the largest unobscured area
         */

        BOX2I bbox = unobscuredPoly.BBox();
        int   step = std::min(bbox.GetWidth(), bbox.GetHeight()) / 10;

        if (step < 20)
            step = 20;

        while (!unobscuredPoly.IsEmpty())
        {
            unobscuredCenter = unobscuredPoly.BBox().Centre();
            unobscuredPoly.Deflate(step, CORNER_STRATEGY::ALLOW_ACUTE_CORNERS, ARC_LOW_DEF);
        }

        SetCenter(aCenter - ToWorld(unobscuredCenter - screenRect.Centre(), false));
    }


    void VIEW::SetLayerOrder(int aLayer, int aRenderingOrder)
    {
        m_layers[aLayer].renderingOrder = aRenderingOrder;

        sortOrderedLayers();
    }


    int VIEW::GetLayerOrder(int aLayer) const
    {
        return m_layers.at(aLayer).renderingOrder;
    }


    void VIEW::SortLayers(std::vector<int>& aLayers) const
    {
        std::sort(aLayers.begin(), aLayers.end(),
            [this](int a, int b)
            {
                return GetLayerOrder(a) > GetLayerOrder(b);
            });
    }


    void VIEW::ReorderLayerData(std::unordered_map<int, int> aReorderMap)
    {
        std::map<int, VIEW_LAYER> new_map;

        for (auto& [_, layer] : m_layers)
        {
            auto reorder_it = aReorderMap.find(layer.id);

            // If the layer is not in the reorder map or if it is mapped to itself,
            // just copy the layer to the new map.
            if (reorder_it == aReorderMap.end() || reorder_it->second == layer.id)
            {
                new_map.emplace(layer.id, layer);
                continue;
            }

            auto [new_it, __] = new_map.emplace(reorder_it->second, layer);
            new_it->second.id = reorder_it->second;
        }

        // Transfer reordered data (using the copy assignment operator ):
        m_layers = new_map;

        sortOrderedLayers();

        for (VIEW_ITEM* item : *m_allItems)
        {
            if (!item)
                continue;

            VIEW_ITEM_DATA* viewData = item->viewPrivData();

            if (!viewData)
                continue;

            std::vector<int> layers = item->ViewGetLayers();
            viewData->saveLayers(layers);

            viewData->reorderGroups(aReorderMap);

            viewData->m_requiredUpdate |= COLOR;
        }

        UpdateItems();
    }


    struct VIEW::UPDATE_COLOR_VISITOR
    {
        UPDATE_COLOR_VISITOR(int aLayer, PAINTER* aPainter, GAL* aGal) :
            layer(aLayer),
            painter(aPainter),
            gal(aGal)
        {
        }

        bool operator()(VIEW_ITEM* aItem)
        {
            // Obtain the color that should be used for coloring the item
            const COLOR4D color = painter->GetSettings()->GetColor(aItem, layer);
            int           group = aItem->viewPrivData()->getGroup(layer);

            if (group >= 0)
                gal->ChangeGroupColor(group, color);

            return true;
        }

        int layer;
        PAINTER* painter;
        GAL* gal;
    };


    void VIEW::UpdateLayerColor(int aLayer)
    {
        // There is no point in updating non-cached layers
        if (!IsCached(aLayer))
            return;

        BOX2I r;

        r.SetMaximum();

        if (m_gal->IsVisible())
        {
            GAL_UPDATE_CONTEXT ctx(m_gal);

            UPDATE_COLOR_VISITOR visitor(aLayer, m_painter, m_gal);
            m_layers[aLayer].items->Query(r, visitor);
            MarkTargetDirty(m_layers[aLayer].target);
        }
    }


    void VIEW::UpdateAllLayersColor()
    {
        if (m_gal->IsVisible())
        {
            GAL_UPDATE_CONTEXT ctx(m_gal);

            for (VIEW_ITEM* item : *m_allItems)
            {
                if (!item)
                    continue;

                VIEW_ITEM_DATA* viewData = item->viewPrivData();

                if (!viewData)
                    continue;

                for (int layer : viewData->m_layers)
                {
                    const COLOR4D color = m_painter->GetSettings()->GetColor(item, layer);
                    int           group = viewData->getGroup(layer);

                    if (group >= 0)
                        m_gal->ChangeGroupColor(group, color);
                }
            }
        }

        MarkDirty();
    }


    struct VIEW::UPDATE_DEPTH_VISITOR
    {
        UPDATE_DEPTH_VISITOR(int aLayer, int aDepth, GAL* aGal) :
            layer(aLayer),
            depth(aDepth),
            gal(aGal)
        {
        }

        bool operator()(VIEW_ITEM* aItem)
        {
            int group = aItem->viewPrivData()->getGroup(layer);

            if (group >= 0)
                gal->ChangeGroupDepth(group, depth);

            return true;
        }

        int layer, depth;
        GAL* gal;
    };


    int VIEW::GetTopLayer() const
    {
        if (m_topLayers.size() == 0)
            return 0;

        return *m_topLayers.begin();
    }


    void VIEW::SetTopLayer(int aLayer, bool aEnabled)
    {
        if (aEnabled)
        {
            if (m_topLayers.count(aLayer) == 1)
                return;

            m_topLayers.insert(aLayer);

            // Move the layer closer to front
            if (m_enableOrderModifier)
                m_layers[aLayer].renderingOrder += TOP_LAYER_MODIFIER;
        }
        else
        {
            if (m_topLayers.count(aLayer) == 0)
                return;

            m_topLayers.erase(aLayer);

            // Restore the previous rendering order
            if (m_enableOrderModifier)
                m_layers[aLayer].renderingOrder -= TOP_LAYER_MODIFIER;
        }
    }


    void VIEW::EnableTopLayer(bool aEnable)
    {
        if (aEnable == m_enableOrderModifier)
            return;

        m_enableOrderModifier = aEnable;

        std::set<unsigned int>::iterator it;

        if (aEnable)
        {
            for (it = m_topLayers.begin(); it != m_topLayers.end(); ++it)
                m_layers[*it].renderingOrder += TOP_LAYER_MODIFIER;
        }
        else
        {
            for (it = m_topLayers.begin(); it != m_topLayers.end(); ++it)
                m_layers[*it].renderingOrder -= TOP_LAYER_MODIFIER;
        }

        UpdateAllLayersOrder();
        UpdateAllLayersColor();
    }


    void VIEW::ClearTopLayers()
    {
        std::set<unsigned int>::iterator it;

        if (m_enableOrderModifier)
        {
            // Restore the previous rendering order for layers that were marked as top
            for (it = m_topLayers.begin(); it != m_topLayers.end(); ++it)
                m_layers[*it].renderingOrder -= TOP_LAYER_MODIFIER;
        }

        m_topLayers.clear();
    }


    void VIEW::UpdateAllLayersOrder()
    {
        sortOrderedLayers();

        if (m_gal->IsVisible())
        {
            GAL_UPDATE_CONTEXT ctx(m_gal);

            for (VIEW_ITEM* item : *m_allItems)
            {
                if (!item)
                    continue;

                VIEW_ITEM_DATA* viewData = item->viewPrivData();

                if (!viewData)
                    continue;

                for (int layer : viewData->m_layers)
                {
                    int group = viewData->getGroup(layer);

                    if (group >= 0)
                        m_gal->ChangeGroupDepth(group, m_layers[layer].renderingOrder);
                }
            }
        }

        MarkDirty();
    }


    struct VIEW::DRAW_ITEM_VISITOR
    {
        DRAW_ITEM_VISITOR(VIEW* aView, int aLayer, bool aUseDrawPriority, bool aReverseDrawOrder) :
            view(aView),
            layer(aLayer),
            useDrawPriority(aUseDrawPriority),
            reverseDrawOrder(aReverseDrawOrder),
            drawForcedTransparent(false),
            foundForcedTransparent(false)
        {
        }

        bool operator()(VIEW_ITEM* aItem)
        {
            if (!aItem->viewPrivData()) return false;

            if (aItem->m_forcedTransparency > 0 && !drawForcedTransparent)
            {
                foundForcedTransparent = true;
                return true;
            }

            const double itemLOD = aItem->ViewGetLOD(layer, view);

            // Conditions that have to be fulfilled for an item to be drawn
            bool drawCondition = aItem->viewPrivData()->isRenderable() && itemLOD < view->m_scale;

            if (!drawCondition)
                return true;

            if (useDrawPriority)
                drawItems.push_back(aItem);
            else
                view->draw(aItem, layer);

            return true;
        }

        void deferredDraw()
        {
            if (reverseDrawOrder)
            {
                std::sort(drawItems.begin(), drawItems.end(),
                    [](VIEW_ITEM* a, VIEW_ITEM* b) -> bool
                    {
                        return b->viewPrivData()->m_drawPriority
                            < a->viewPrivData()->m_drawPriority;
                    });
            }
            else
            {
                std::sort(drawItems.begin(), drawItems.end(),
                    [](VIEW_ITEM* a, VIEW_ITEM* b) -> bool
                    {
                        return a->viewPrivData()->m_drawPriority
                            < b->viewPrivData()->m_drawPriority;
                    });
            }

            for (VIEW_ITEM* item : drawItems)
                view->draw(item, layer);
        }

        VIEW* view;
        int layer, layers[VIEW_MAX_LAYERS];
        bool useDrawPriority, reverseDrawOrder;
        std::vector<VIEW_ITEM*> drawItems;
        bool drawForcedTransparent;
        bool foundForcedTransparent;
    };


    void VIEW::redrawRect(const BOX2I& aRect)
    {
        for (VIEW_LAYER* l : m_orderedLayers)
        {
            if (l->visible && IsTargetDirty(l->target) && areRequiredLayersEnabled(l->id))
            {
                DRAW_ITEM_VISITOR drawFunc(this, l->id, m_useDrawPriority, m_reverseDrawOrder);

                m_gal->SetTarget(l->target);
                m_gal->SetLayerDepth(l->renderingOrder);

                // Differential layer also work for the negatives, since both special layer types
                // will composite on separate layers (at least in Cairo)
                if (l->diffLayer)
                    m_gal->StartDiffLayer();
                else if (l->hasNegatives)
                    m_gal->StartNegativesLayer();

                l->items->Query(aRect, drawFunc);

                if (m_useDrawPriority)
                    drawFunc.deferredDraw();

                if (l->diffLayer)
                    m_gal->EndDiffLayer();
                else if (l->hasNegatives)
                    m_gal->EndNegativesLayer();

                if (drawFunc.foundForcedTransparent)
                {
                    drawFunc.drawForcedTransparent = true;

                    m_gal->SetTarget(TARGET_NONCACHED);
                    m_gal->EnableDepthTest(true);
                    m_gal->SetLayerDepth(l->renderingOrder);

                    l->items->Query(aRect, drawFunc);
                }
            }
        }
    }


    void VIEW::draw(VIEW_ITEM* aItem, int aLayer, bool aImmediate)
    {
        VIEW_ITEM_DATA* viewData = aItem->viewPrivData();

        if (!viewData)
            return;

        if (IsCached(aLayer) && !aImmediate)
        {
            // Draw using cached information or create one
            int group = viewData->getGroup(aLayer);

            if (group >= 0)
                m_gal->DrawGroup(group);
            else
                Update(aItem);
        }
        else
        {
            // Immediate mode
            if (!m_painter->Draw(aItem, aLayer))
                aItem->ViewDraw(aLayer, this);  // Alternative drawing method
        }
    }


    void VIEW::draw(VIEW_ITEM* aItem, bool aImmediate)
    {
        std::vector<int> layers = aItem->ViewGetLayers();

        // Sorting is needed for drawing order dependent GALs (like Cairo)
        if (!m_gal || !m_gal->IsOpenGlEngine())
            SortLayers(layers);

        for (int layer : layers)
        {
            auto it = m_layers.find(layer);

            if (it == m_layers.end())
                continue;

            if (m_gal)
                m_gal->SetLayerDepth(it->second.renderingOrder);

            draw(aItem, layer, aImmediate);
        }
    }


    //void VIEW::draw(VIEW_GROUP* aGroup, bool aImmediate)
    //{
        //for (unsigned int i = 0; i < aGroup->GetSize(); i++)
        //    draw(aGroup->GetItem(i), aImmediate);
    //}


    struct VIEW::RECACHE_ITEM_VISITOR
    {
        RECACHE_ITEM_VISITOR(VIEW* aView, GAL* aGal, int aLayer) :
            view(aView),
            gal(aGal),
            layer(aLayer)
        {
        }

        bool operator()(VIEW_ITEM* aItem)
        {
            VIEW_ITEM_DATA* viewData = aItem->viewPrivData();

            if (!viewData)
                return false;

            // Remove previously cached group
            int group = viewData->getGroup(layer);

            if (group >= 0)
                gal->DeleteGroup(group);

            viewData->setGroup(layer, -1);
            view->Update(aItem);

            return true;
        }

        VIEW* view;
        GAL* gal;
        int layer;
    };


    void VIEW::Clear()
    {
        BOX2I r;
        r.SetMaximum();
        m_allItems->clear();

        for (auto& [_, layer] : m_layers)
            layer.items->RemoveAll();

        m_nextDrawPriority = 0;

        m_gal->ClearCache();
    }


    void VIEW::ClearTargets()
    {
        if (IsTargetDirty(TARGET_CACHED) || IsTargetDirty(TARGET_NONCACHED))
        {
            // TARGET_CACHED and TARGET_NONCACHED have to be redrawn together, as they contain
            // layers that rely on each other (eg. netnames are noncached, but tracks - are cached)
            m_gal->ClearTarget(TARGET_NONCACHED);
            m_gal->ClearTarget(TARGET_CACHED);

            MarkDirty();
        }

        if (IsTargetDirty(TARGET_OVERLAY))
        {
            m_gal->ClearTarget(TARGET_OVERLAY);
        }
    }


    void VIEW::Redraw()
    {
#ifdef KICAD_GAL_PROFILE
        PROF_TIMER totalRealTime;
#endif /* KICAD_GAL_PROFILE */

        VECTOR2D screenSize = m_gal->GetScreenPixelSize();
        BOX2D    rect(ToWorld(VECTOR2D(0, 0)),
            ToWorld(screenSize) - ToWorld(VECTOR2D(0, 0)));

        rect.Normalize();
        BOX2I recti = BOX2ISafe(rect);

        redrawRect(recti);

        // All targets were redrawn, so nothing is dirty
        //MarkClean();

#ifdef KICAD_GAL_PROFILE
        totalRealTime.Stop();
        wxLogTrace(traceGalProfile, wxS("VIEW::Redraw(): %.1f ms"), totalRealTime.msecs());
#endif /* KICAD_GAL_PROFILE */
    }


    const VECTOR2I& VIEW::GetScreenPixelSize() const
    {
        return m_gal->GetScreenPixelSize();
    }


    struct VIEW::CLEAR_LAYER_CACHE_VISITOR
    {
        CLEAR_LAYER_CACHE_VISITOR(VIEW* aView) :
            view(aView)
        {
        }

        bool operator()(VIEW_ITEM* aItem)
        {
            aItem->viewPrivData()->deleteGroups();

            return true;
        }

        VIEW* view;
    };


    void VIEW::clearGroupCache()
    {
        BOX2I r;

        r.SetMaximum();
        CLEAR_LAYER_CACHE_VISITOR visitor(this);

        for (auto& [_, layer] : m_layers)
            layer.items->Query(r, visitor);
    }


    void VIEW::invalidateItem(VIEW_ITEM* aItem, int aUpdateFlags)
    {
        if (aUpdateFlags & INITIAL_ADD)
        {
            // Don't update layers or bbox, since it was done in VIEW::Add()
            // Now that we have initialized, set flags to ALL for the code below
            aUpdateFlags = ALL;
        }
        else
        {
            // updateLayers updates geometry too, so we do not have to update both of them at the
            // same time
            if (aUpdateFlags & LAYERS)
                updateLayers(aItem);
            else if (aUpdateFlags & GEOMETRY)
                updateBbox(aItem);
        }

        std::vector<int> layers = aItem->ViewGetLayers();

        // Iterate through layers used by the item and recache it immediately
        for (int layer : layers)
        {
            if (IsCached(layer))
            {
                if (aUpdateFlags & (GEOMETRY | LAYERS | REPAINT))
                    updateItemGeometry(aItem, layer);
                else if (aUpdateFlags & COLOR)
                    updateItemColor(aItem, layer);
            }

            // Mark those layers as dirty, so the VIEW will be refreshed
            MarkTargetDirty(m_layers[layer].target);
        }

        aItem->viewPrivData()->clearUpdateFlags();
    }


    void VIEW::sortOrderedLayers()
    {
        int n = 0;

        m_orderedLayers.resize(m_layers.size());

        for (auto& [layer_id, layer] : m_layers)
            m_orderedLayers[n++] = &layer;

        sort(m_orderedLayers.begin(), m_orderedLayers.end(), compareRenderingOrder);

        MarkDirty();
    }


    void VIEW::updateItemColor(VIEW_ITEM* aItem, int aLayer)
    {
        VIEW_ITEM_DATA* viewData = aItem->viewPrivData();
        if (!IsCached(aLayer)) return;

        if (!viewData)
            return;

        // Obtain the color that should be used for coloring the item on the specific layerId
        const COLOR4D color = m_painter->GetSettings()->GetColor(aItem, aLayer);
        int group = viewData->getGroup(aLayer);

        // Change the color, only if it has group assigned
        if (group >= 0)
            m_gal->ChangeGroupColor(group, color);
    }


    void VIEW::updateItemGeometry(VIEW_ITEM* aItem, int aLayer)
    {
        VIEW_ITEM_DATA* viewData = aItem->viewPrivData();

        if (!viewData)
            return;

        auto it = m_layers.find(aLayer);

        if (it == m_layers.end())
            return;

        VIEW_LAYER& l = it->second;

        // Save the extra map lookup in IsCached by open coding here
        if (l.target != TARGET_CACHED)
            return;

        m_gal->SetTarget(l.target);
        m_gal->SetLayerDepth(l.renderingOrder);

        // Redraw the item from scratch
        int group = viewData->getGroup(aLayer);

        if (group >= 0)
            m_gal->DeleteGroup(group);

        group = m_gal->BeginGroup();
        viewData->setGroup(aLayer, group);

        if (!m_painter->Draw(aItem, aLayer))
            aItem->ViewDraw(aLayer, this); // Alternative drawing method

        m_gal->EndGroup();
    }


    void VIEW::updateBbox(VIEW_ITEM* aItem)
    {
        std::vector<int> layers = aItem->ViewGetLayers();

        Q_ASSERT(aItem->m_viewPrivData); //must have a viewPrivData

        const BOX2I  new_bbox = aItem->ViewBBox();
        const BOX2I* old_bbox = &aItem->m_viewPrivData->m_bbox;
        aItem->m_viewPrivData->m_bbox = new_bbox;

        for (int layer : layers)
        {
            auto it = m_layers.find(layer);

            if (it == m_layers.end())
                continue;

            VIEW_LAYER& l = it->second;
            l.items->Remove(aItem, old_bbox);
            l.items->Insert(aItem, new_bbox);
            MarkTargetDirty(l.target);
        }
    }


    void VIEW::updateLayers(VIEW_ITEM* aItem)
    {
        VIEW_ITEM_DATA* viewData = aItem->viewPrivData();

        if (!viewData)
            return;

        // Remove the item from previous layer set
        const BOX2I* old_bbox = &aItem->m_viewPrivData->m_bbox;

        for (int layer : aItem->m_viewPrivData->m_layers)
        {
            auto it = m_layers.find(layer);

            if (it == m_layers.end())
                continue;

            VIEW_LAYER& l = it->second;
            l.items->Remove(aItem, old_bbox);
            MarkTargetDirty(l.target);

            if (IsCached(l.id))
            {
                // Redraw the item from scratch
                int prevGroup = viewData->getGroup(layer);

                if (prevGroup >= 0)
                {
                    m_gal->DeleteGroup(prevGroup);
                    viewData->setGroup(l.id, -1);
                }
            }
        }

        const BOX2I new_bbox = aItem->ViewBBox();
        aItem->m_viewPrivData->m_bbox = new_bbox;

        // Add the item to new layer set
        std::vector<int> layers = aItem->ViewGetLayers();
        viewData->saveLayers(layers);

        for (int layer : layers)
        {
            auto it = m_layers.find(layer);

            if (it == m_layers.end())
                continue;

            VIEW_LAYER& l = it->second;
            l.items->Insert(aItem, new_bbox);
            MarkTargetDirty(l.target);
        }
    }


    bool VIEW::areRequiredLayersEnabled(int aLayerId) const
    {
        auto it = m_layers.find(aLayerId);

        if (it == m_layers.end())
            return false;

        for (int layer : it->second.requiredLayers)
        {
            // That is enough if just one layer is not enabled

            auto it2 = m_layers.find(layer);

            if (it2 == m_layers.end() || !it2->second.visible)
                return false;

            if (!areRequiredLayersEnabled(layer))
                return false;
        }

        return true;
    }


    void VIEW::RecacheAllItems()
    {
        BOX2I r;

        r.SetMaximum();

        for (const auto& [_, l] : m_layers)
        {
            if (IsCached(l.id))
            {
                RECACHE_ITEM_VISITOR visitor(this, m_gal, l.id);
                l.items->Query(r, visitor);
            }
        }
    }


    void VIEW::UpdateItems()
    {
        if (!m_gal->IsVisible() || !m_gal->IsInitialized())
            return;

        unsigned int cntGeomUpdate = 0;
        bool         anyUpdated = false;

        for (VIEW_ITEM* item : *m_allItems)
        {
            if (!item)
                continue;

            auto vpd = item->viewPrivData();

            if (!vpd)
                continue;

            if (vpd->m_requiredUpdate != NONE)
            {
                anyUpdated = true;

                if (vpd->m_requiredUpdate & (GEOMETRY | LAYERS))
                {
                    cntGeomUpdate++;
                }
            }
        }

        unsigned int cntTotal = m_allItems->size();

        double ratio = (double)cntGeomUpdate / (double)cntTotal;

        // Optimization to improve view update time. If a lot of items (say, 30%) have their
        // bboxes/geometry changed it's way faster (around 10 times) to rebuild the R-Trees
        // from scratch rather than update the bbox of each changed item. Pcbnew does multiple
        // full geometry updates during file load, this can save a solid 30 seconds on load time
        // for larger designs...

        if (ratio > 0.3)
        {
            auto allItems = *m_allItems;

            // kill all Rtrees
            for (auto& [_, layer] : m_layers)
                layer.items->RemoveAll();

            // and re-insert items from scratch
            for (VIEW_ITEM* item : allItems)
            {
                if (!item)
                    continue;

                const BOX2I bbox = item->ViewBBox();
                item->m_viewPrivData->m_bbox = bbox;

                std::vector<int> layers = item->ViewGetLayers();
                item->viewPrivData()->saveLayers(layers);

                for (int layer : layers)
                {
                    auto it = m_layers.find(layer);

                    if (it == m_layers.end()) continue;


                    VIEW_LAYER& l = it->second;
                    l.items->Insert(item, bbox);
                    MarkTargetDirty(l.target);
                }

                item->viewPrivData()->m_requiredUpdate &= ~(LAYERS | GEOMETRY);
            }
        }

        if (anyUpdated)
        {
            GAL_UPDATE_CONTEXT ctx(m_gal);

            for (VIEW_ITEM* item : *m_allItems.get())
            {
                if (item && item->viewPrivData() && item->viewPrivData()->m_requiredUpdate != NONE)
                {
                    invalidateItem(item, item->viewPrivData()->m_requiredUpdate);
                    item->viewPrivData()->m_requiredUpdate = NONE;
                }
            }
        }

        spdlog::trace(std::format("[{}] View update: total items %u, geom %u anyUpdated %u\n", traceGalProfile, cntTotal, cntGeomUpdate, (unsigned)anyUpdated));
    }


    void VIEW::UpdateAllItems(int aUpdateFlags)
    {
        for (VIEW_ITEM* item : *m_allItems)
        {
            if (item && item->viewPrivData())
                item->viewPrivData()->m_requiredUpdate |= aUpdateFlags;
        }
    }


    void VIEW::UpdateAllItemsConditionally(int aUpdateFlags,
        std::function<bool(VIEW_ITEM*)> aCondition)
    {
        for (VIEW_ITEM* item : *m_allItems)
        {
            if (!item)
                continue;

            if (aCondition(item))
            {
                if (item->viewPrivData())
                    item->viewPrivData()->m_requiredUpdate |= aUpdateFlags;
            }
        }
    }


    void VIEW::UpdateAllItemsConditionally(std::function<int(VIEW_ITEM*)> aItemFlagsProvider)
    {
        for (VIEW_ITEM* item : *m_allItems)
        {
            if (!item)
                continue;

            if (item->viewPrivData())
                item->viewPrivData()->m_requiredUpdate |= aItemFlagsProvider(item);
        }
    }



    std::unique_ptr<VIEW> VIEW::DataReference() const
    {
        std::unique_ptr<VIEW> ret = std::make_unique<VIEW>();
        ret->m_allItems = m_allItems;
        ret->m_layers = m_layers;
        ret->sortOrderedLayers();
        return ret;
    }


    void VIEW::SetVisible(VIEW_ITEM* aItem, bool aIsVisible)
    {
        VIEW_ITEM_DATA* viewData = aItem->viewPrivData();

        if (!viewData)
            return;

        bool cur_visible = viewData->m_flags & VISIBLE;

        if (cur_visible != aIsVisible)
        {
            if (aIsVisible)
                viewData->m_flags |= VISIBLE;
            else
                viewData->m_flags &= ~VISIBLE;

            Update(aItem, APPEARANCE | COLOR);
        }
    }


    void VIEW::Hide(VIEW_ITEM* aItem, bool aHide, bool aHideOverlay)
    {
        VIEW_ITEM_DATA* viewData = aItem->viewPrivData();

        if (!viewData)
            return;

        if (!(viewData->m_flags & VISIBLE))
            return;

        if (aHideOverlay)
            viewData->m_flags |= OVERLAY_HIDDEN;

        if (aHide)
            viewData->m_flags |= HIDDEN;
        else
            viewData->m_flags &= ~(HIDDEN | OVERLAY_HIDDEN);

        Update(aItem, APPEARANCE);
    }


    bool VIEW::IsVisible(const VIEW_ITEM* aItem) const
    {
        const VIEW_ITEM_DATA* viewData = aItem->viewPrivData();

        return viewData && (viewData->m_flags & VISIBLE);
    }


    bool VIEW::IsHiddenOnOverlay(const VIEW_ITEM* aItem) const
    {
        const VIEW_ITEM_DATA* viewData = aItem->viewPrivData();

        return viewData && (viewData->m_flags & OVERLAY_HIDDEN);
    }


    bool VIEW::HasItem(const VIEW_ITEM* aItem) const
    {
        const VIEW_ITEM_DATA* viewData = aItem->viewPrivData();

        return viewData && viewData->m_view == this;
    }


    void VIEW::Update(const VIEW_ITEM* aItem) const
    {
        Update(aItem, ALL);
    }


    void VIEW::Update(const VIEW_ITEM* aItem, int aUpdateFlags) const
    {
        VIEW_ITEM_DATA* viewData = aItem->viewPrivData();

        if (!viewData)
            return;

        assert(aUpdateFlags != NONE);

        viewData->m_requiredUpdate |= aUpdateFlags;
    }


    //std::shared_ptr<VIEW_OVERLAY> VIEW::MakeOverlay()
    //{
    //    std::shared_ptr<VIEW_OVERLAY> overlay = std::make_shared<VIEW_OVERLAY>();

    //    //Add(overlay.get());
    //    return overlay;
    //}


    void VIEW::ClearPreview()
    {
        //if (!m_preview)
        //    return;

        //m_preview->Clear();

        for (VIEW_ITEM* item : m_ownedItems)
            delete item;

        m_ownedItems.clear();
        //Update(m_preview.get());
    }


    void VIEW::InitPreview()
    {
        //m_preview.reset(new KIGFX::VIEW_GROUP());
        //Add(m_preview.get());
    }


    void VIEW::AddToPreview(VIEW_ITEM* aItem, bool aTakeOwnership)
    {
        //Hide(aItem, false);
        //m_preview->Add(aItem);

        //if (aTakeOwnership)
        //    m_ownedItems.push_back(aItem);

        //SetVisible(m_preview.get(), true);
        //Hide(m_preview.get(), false);
        //Update(m_preview.get());
    }


    void VIEW::ShowPreview(bool aShow)
    {
        //SetVisible(m_preview.get(), aShow);
    }


} // namespace KIGFX
