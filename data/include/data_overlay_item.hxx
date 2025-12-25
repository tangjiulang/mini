#ifndef PREVIEW_SIMPLE_OUTLINE_ITEM__H_
#define PREVIEW_SIMPLE_OUTLINE_ITEM__H_

#include <data_board_item.hxx>

#include <layer_ids.hxx>
#include <color4d.hxx>

namespace KIGFX
{

class VIEW;
class GAL;

    /**
        * SIMPLE_OVERLAY_ITEM is class that represents a visual area drawn on
        * a canvas, used to temporarily demarcate an area or show something
        * on an overlay. An example could be the drag select lasso box.
        *
        * This class is pretty generic in terms of what the area looks like.
        * It provides a fill, stroke and width, which is probably sufficient
        * for most simple previews, but the inheritor has freedom to override
        * this.
        *
        * If this doesn't suit a particular preview, it may mean you should
        * implement your own BOARD_ITEM derivative rather than inheriting this.
        */
    class SIMPLE_OVERLAY_ITEM : public BOARD_ITEM
    {
    public:

        SIMPLE_OVERLAY_ITEM();

        /**
            * Set the overlay layer only. You can override this if
            * you have more layers to draw on.
            */
        std::vector<int> ViewGetLayers() const override;

        /**
            * Draw the preview - this is done by calling the two functions:
            * setupGal() and drawPreviewShape(). If you need more than this,
            * or direct access to the VIEW, you probably should make a new
            *.
            */
        void ViewDraw(int aLayer, KIGFX::VIEW* aView) const override;


#if defined(DEBUG)
        void Show(int x, std::ostream& st) const override
        {
        }
#endif

        /**
            * Get class name
            * @return  string "SIMPLE_OVERLAY_ITEM"
            */
        virtual std::string GetClass() const override
        {
            return "SIMPLE_OVERLAY_ITEM";
        }

        ///< Set the stroke color to set before drawing preview
        void SetStrokeColor(const COLOR4D& aNewColor)
        {
            m_strokeColor = aNewColor;
        }

        ///< Set the fill color to set before drawing preview
        void SetFillColor(const COLOR4D& aNewColor)
        {
            m_fillColor = aNewColor;
        }

        ///< Set the line width to set before drawing preview
        void SetLineWidth(double aNewWidth)
        {
            m_lineWidth = aNewWidth;
        }

    private:

        /**
            * Set up the GAL canvas - this provides a default implementation,
            * that sets fill, stroke and width.
            *
            * If that's not suitable, you can set more options in
            * updatePreviewShape(), but you might find that defining a new
            * EDA_ITEM derivative is easier for heavily customized cases.
            */
        void setupGal(KIGFX::GAL& aGal) const;

        /**
            * Draw the preview onto the given GAL. setupGal() will be called before this function.
            *
            * Subclasses should implement this in terms of their own graphical
            * data.
            */
        virtual void drawPreviewShape(KIGFX::VIEW* aView) const {};


        COLOR4D m_fillColor;
        COLOR4D m_strokeColor;
        double  m_lineWidth;
    };

} // KIGFX

#endif  // PREVIEW_SIMPLE_OUTLINE_ITEM__H_
