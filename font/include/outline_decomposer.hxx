#ifndef OUTLINE_DECOMPOSER_H
#define OUTLINE_DECOMPOSER_H

#include <vector>
#ifdef _MSC_VER
#include <ft2build.h>
#else
#include <freetype2/ft2build.h>
#endif
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include <box2.hxx>
#include <vector2d.hxx>
#include <glyph.hxx>

namespace KIFONT
{

constexpr int GLYPH_DEFAULT_DPI = 72;  ///< FreeType default

// The FreeType default of 72 DPI is not enough for outline decomposition;
// so we'll use something larger than that.
constexpr int GLYPH_RESOLUTION  = 1152;
constexpr double GLYPH_SIZE_SCALER = GLYPH_DEFAULT_DPI / (double) GLYPH_RESOLUTION;

struct CONTOUR
{
    std::vector<VECTOR2D> m_Points;
    int                   m_Winding = 0;
    FT_Orientation        m_Orientation;
};

struct GLYPH_DATA
{
    std::vector<CONTOUR> m_Contours;

    // Cache of the triangulation data.  We'll use this as a hint for triangulating the actual
    // OUTLINE_GLYPHs.
    std::vector<std::unique_ptr<SHAPE_POLY_SET::TRIANGULATED_POLYGON>> m_TriangulationData;
};


class OUTLINE_DECOMPOSER
{
public:
    OUTLINE_DECOMPOSER( FT_Outline& aOutline );

    bool OutlineToSegments( std::vector<CONTOUR>* aContours );

private:
    void newContour();

    void addContourPoint( const VECTOR2D& p );

    /**
     * @return 1 if aContour is in clockwise order, -1 if it is in counterclockwise order,
     *         or 0 if the winding can't be determined.
     */
    int winding( const std::vector<VECTOR2D>& aContour ) const;

    inline static unsigned int onCurve( char aTags )
    {
        return aTags & 0x1;
    }

    inline static unsigned int thirdOrderBezierPoint( char aTags )
    {
        return onCurve( aTags ) ? 0 : aTags & 0x2;
    }

    inline static unsigned int secondOrderBezierPoint( char aTags )
    {
        return onCurve( aTags ) ? 0 : !thirdOrderBezierPoint( aTags );
    }

    inline static unsigned int hasDropout( char aTags )
    {
        return aTags & 0x4;
    }

    inline static unsigned int dropoutMode( char aTags )
    {
        return hasDropout( aTags ) ? ( aTags & 0x38 ) : 0;
    }

    // FT_Outline_Decompose callbacks
    static int moveTo( const FT_Vector* aEndPoint, void* aCallbackData );

    static int lineTo( const FT_Vector* aEndPoint, void* aCallbackData );

    static int quadraticTo( const FT_Vector* aControlPoint, const FT_Vector* aEndPoint,
                            void* aCallbackData );

    static int cubicTo( const FT_Vector* aFirstControlPoint, const FT_Vector* aSecondControlPoint,
                        const FT_Vector* aEndPoint, void* aCallbackData );

private:
    FT_Outline&           m_outline;
    std::vector<CONTOUR>* m_contours;

    VECTOR2D              m_lastEndPoint;
};

} //namespace KIFONT

#endif      // OUTLINE_DECOMPOSER_H
