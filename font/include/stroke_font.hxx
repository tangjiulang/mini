#ifndef STROKE_FONT_H
#define STROKE_FONT_H

#include <gal/include/gal.hxx>
#include <map>
#include <deque>
#include <algorithm>
#include <utf8.hxx>
#include <box2.hxx>
#include <font.hxx>

namespace KIGFX
{
class GAL;
}

namespace KIFONT
{
/**
 * Implement a stroke font drawing.
 *
 * A stroke font is composed of lines.
 */
class STROKE_FONT : public FONT
{
public:
    STROKE_FONT();

    bool IsStroke() const override { return true; }

    /**
     * Load a stroke font.
     *
     * @param aFontName is the name of the font. If empty, the standard KiCad stroke font is
     *                  loaded.
     */
    static STROKE_FONT* LoadFont( const std::string& aFontName );

    /**
     * Compute the distance (interline) between 2 lines of text (for multiline texts).
     *
     * This is the distance between baselines, not the space between line bounding boxes.
     */
    double GetInterline( double aGlyphHeight, const METRICS& aFontMetrics ) const override;

    VECTOR2I GetTextAsGlyphs( BOX2I* aBoundingBox, std::vector<std::unique_ptr<GLYPH>>* aGlyphs,
                              const std::string& aText, const VECTOR2I& aSize,
                              const VECTOR2I& aPosition, const EDA_ANGLE& aAngle, bool aMirror,
                              const VECTOR2I& aOrigin, TEXT_STYLE_FLAGS aTextStyle ) const override;

    unsigned GetGlyphCount() const;

    const STROKE_GLYPH* GetGlyph( unsigned aIndex ) const;

    const BOX2D& GetGlyphBoundingBox( unsigned aIndex ) const;

private:
    /**
     * Load the standard KiCad stroke font.
     *
     * @param aNewStrokeFont is the pointer to the font data.
     * @param aNewStrokeFontSize is the size of the font data.
     */
    void loadNewStrokeFont( const char* const aNewStrokeFont[], int aNewStrokeFontSize );

private:
    const std::vector<std::shared_ptr<GLYPH>>* m_glyphs;
    const std::vector<BOX2D>*                  m_glyphBoundingBoxes;
    double                                     m_maxGlyphWidth;
};

} //namespace KIFONT

#endif // STROKE_FONT_H
