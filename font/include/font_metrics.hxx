#ifndef FONT_METRICS_H_
#define FONT_METRICS_H_

#include <gal/include/gal.hxx>

namespace KIFONT
{

class METRICS
{
public:
    /**
     * Compute the vertical position of an overbar.  This is the distance between the text
     * baseline and the overbar.
     */
    double GetOverbarVerticalPosition( double aGlyphHeight ) const
    {
        return aGlyphHeight * m_OverbarHeight;
    }

    /**
     * Compute the vertical position of an underline.  This is the distance between the text
     * baseline and the underline.
     */
    double GetUnderlineVerticalPosition( double aGlyphHeight ) const
    {
        return aGlyphHeight * m_UnderlineOffset;
    }

    double GetInterline( double aFontHeight ) const
    {
        return aFontHeight * m_InterlinePitch;
    }

    static const METRICS& Default();

public:
    double m_InterlinePitch  =  1.68;
    double m_OverbarHeight   =  1.23;
    double m_UnderlineOffset = -0.16;
};

} // namespace KIFONT

#endif // FONT_METRICS_H_
