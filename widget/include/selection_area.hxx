#pragma once

#include <data_overlay_item.hxx>
#include <shape_line_chain.hxx>
#include "view.hxx"

enum class SELECTION_MODE
{
    INSIDE_RECTANGLE,
    TOUCHING_RECTANGLE,
    INSIDE_LASSO,
    TOUCHING_LASSO
};

namespace KIGFX
{
    class GAL;

    /**
        * Represent a selection area (currently a rectangle) in a VIEW, drawn corner-to-corner between
        * two points.
        *
        * This is useful when selecting a rectangular area, for lasso-select or zooming, for example.
        */
    class SELECTION_AREA : public SIMPLE_OVERLAY_ITEM
    {
    public:
        static const int SelectionLayer = LAYER_GP_OVERLAY;

        SELECTION_AREA();

        const BOX2I ViewBBox() const override;

        ///< Set the origin of the rectangle (the fixed corner)
        void SetOrigin(const VECTOR2I& aOrigin)
        {
            m_origin = aOrigin;
        }

        /**
            * Set the current end of the rectangle (the corner that moves with the cursor.
            */
        void SetEnd(const VECTOR2I& aEnd)
        {
            m_end = aEnd;
        }

        /**
            * @return  string "SELECTION_AREA"
            */
        std::string GetClass() const override
        {
            return std::string("SELECTION_AREA");
        }

        VECTOR2I GetOrigin() const { return m_origin; }

        VECTOR2I GetEnd() const { return m_end; }

        void SetAdditive(bool aAdditive) { m_additive = aAdditive; }
        void SetSubtractive(bool aSubtractive) { m_subtractive = aSubtractive; }
        void SetExclusiveOr(bool aExclusiveOr) { m_exclusiveOr = aExclusiveOr; }

        void              SetMode(SELECTION_MODE aMode) { m_mode = aMode; }
        SELECTION_MODE    GetMode() const { return m_mode; }

        void              SetPoly(SHAPE_LINE_CHAIN& aPoly) { m_shape_poly = aPoly; }
        SHAPE_LINE_CHAIN& GetPoly() { return m_shape_poly; }

        void ViewDraw(int aLayer, KIGFX::VIEW* aView) const override final;

        virtual const BOX2I GetBoundingBox() const override;
    private:

        bool m_additive;
        bool m_subtractive;
        bool m_exclusiveOr;

        SELECTION_MODE   m_mode;

        VECTOR2I         m_origin, m_end; // Used for box selection
        SHAPE_LINE_CHAIN m_shape_poly;    // Used for lasso selection
    };
        
} // KIGFX

