#pragma once

#include <boost/geometry.hpp>

#include <box2.hxx>

namespace KIGFX
{
    namespace bg = boost::geometry;
    namespace bgi = boost::geometry::index;
    using Box = bg::model::box<bg::model::point<double, 2, bg::cs::cartesian>>;
    using Point2D = bg::model::point<double, 2, bg::cs::cartesian>;
    using Value = std::pair<Box, VIEW_ITEM*>;

    /**
     * Implement an non-owning R-tree for fast spatial indexing of VIEW items.
     */
    class VIEW_RTREE
    {
    public:
        /**
         * Insert an item into the tree.
         *
         * Item's bounding box is taken via its ViewBBox() method.
         */
        void Insert(VIEW_ITEM* aItem, const BOX2I& bbox)
        {
            const int       mmin[2] = { std::min(bbox.GetX(), bbox.GetRight()),
                                        std::min(bbox.GetY(), bbox.GetBottom()) };
            const int       mmax[2] = { std::max(bbox.GetX(), bbox.GetRight()),
                                        std::max(bbox.GetY(), bbox.GetBottom()) };

            rtree.insert(Value({ Box(Point2D(mmin[0], mmin[1]), Point2D(mmax[0], mmax[1])), aItem }));
        }

        /**
         * Remove an item from the tree.
         *
         * Removal is done by comparing pointers, attempting to remove a copy of the item will fail.
         */
        void Remove(VIEW_ITEM* aItem, const BOX2I* aBbox)
        {
            // const BOX2I&    bbox    = aItem->ViewBBox();

            if (aBbox)
            {
                const int mmin[2] = { std::min(aBbox->GetX(), aBbox->GetRight()),
                                      std::min(aBbox->GetY(), aBbox->GetBottom()) };
                const int mmax[2] = { std::max(aBbox->GetX(), aBbox->GetRight()),
                                      std::max(aBbox->GetY(), aBbox->GetBottom()) };
                rtree.remove(Value({ Box(Point2D(mmin[0], mmin[1]), Point2D(mmax[0], mmax[1])), aItem }));
                return;
            }

            // FIXME: use cached bbox or ptr_map to speed up pointer <-> node lookups.
            const int       mmin[2] = { INT_MIN, INT_MIN };
            const int       mmax[2] = { INT_MAX, INT_MAX };

            rtree.remove(Value({ Box(Point2D(mmin[0], mmin[1]), Point2D(mmax[0], mmax[1])), aItem }));
        }

        /**
         * Execute a function object \a aVisitor for each item whose bounding box intersects
         * with \a aBounds.
         */
        template <class Visitor>
        void Query(const BOX2I& aBounds, Visitor& aVisitor) const
        {
            int   mmin[2] = { std::min(aBounds.GetX(), aBounds.GetRight()),
                              std::min(aBounds.GetY(), aBounds.GetBottom()) };
            int   mmax[2] = { std::max(aBounds.GetX(), aBounds.GetRight()),
                              std::max(aBounds.GetY(), aBounds.GetBottom()) };

            // We frequently use the maximum bounding box to recache all items
            // or for any item that overflows the integer width limits of BBOX2I
            // in this case, we search the full rtree whose bounds are absolute
            // coordinates rather than relative
            BOX2I max_box;
            max_box.SetMaximum();

            if (aBounds == max_box)
            {
                mmin[0] = mmin[1] = INT_MIN;
                mmax[0] = mmax[1] = INT_MAX;
            }
            std::vector<Value> val;
            rtree.query(bgi::intersects(Box(Point2D(mmin[0], mmin[1]), Point2D(mmax[0], mmax[1]))), std::back_inserter(val));
            for (auto [box, item] : val) {
                aVisitor(item);
            }
            //rtree.query(
            //    bgi::intersects(query_box),
            //    boost::make_function_output_iterator([&](auto const& v) { aVisitor(v.second); })
            //);
        }
        void RemoveAll() {
            rtree.clear();
        }
    private:
        bgi::rtree<Value, bgi::quadratic<16>> rtree;
    };
} // namespace KIGFX

