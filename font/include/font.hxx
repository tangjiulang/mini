#ifndef FONT_H_
#define FONT_H_

#include <gal/include/graphics_abstraction_layer.hxx>

#include <iostream>
#include <map>
#include <algorithm>
#include <font_metrics.hxx>
#include <glyph.hxx>
#include <text_attributes.hxx>
#include <string>


namespace KIGFX
{
class GAL;
}


enum TEXT_STYLE
{
    BOLD        = 1,
    ITALIC      = 1 << 1,
    SUBSCRIPT   = 1 << 2,
    SUPERSCRIPT = 1 << 3,
    OVERBAR     = 1 << 4,
    UNDERLINE   = 1 << 5
};


/**
 * Tilt factor for italic style (this is the scaling factor on dY relative coordinates to give
 * a tilted shape).
 * This is applied directly to stroke fonts, and is used as an estimate for outline fonts (which
 * have the actual tilt built in to their polygonal glyph outlines).
 */
static constexpr double ITALIC_TILT = 1.0 / 8;


using TEXT_STYLE_FLAGS = unsigned int;


inline bool IsBold( TEXT_STYLE_FLAGS aFlags )
{
    return aFlags & TEXT_STYLE::BOLD;
}


inline bool IsItalic( TEXT_STYLE_FLAGS aFlags )
{
    return aFlags & TEXT_STYLE::ITALIC;
}


inline bool IsSuperscript( TEXT_STYLE_FLAGS aFlags )
{
    return aFlags & TEXT_STYLE::SUPERSCRIPT;
}


inline bool IsSubscript( TEXT_STYLE_FLAGS aFlags )
{
    return aFlags & TEXT_STYLE::SUBSCRIPT;
}


namespace KIFONT
{
/**
 * FONT is an abstract base class for both outline and stroke fonts
 */
class FONT
{
public:
    explicit FONT();

    virtual ~FONT()
    { }

    virtual bool IsStroke() const  { return false; }
    virtual bool IsOutline() const { return false; }
    virtual bool IsBold() const    { return false; }
    virtual bool IsItalic() const  { return false; }

    static FONT* GetFont(const std::string& aFontName = "", bool aBold = false,
                          bool aItalic = false,
                          const std::vector<std::string>* aEmbeddedFiles = nullptr,
                          bool aForDrawingSheet = false );
    static bool IsStroke( const std::string& aFontName );

    const std::string& GetName() const { return m_fontName; };
    inline const char* NameAsToken() const { return GetName().data(); }

    /**
     * Draw a string.
     *
     * @param aGal is the graphics context.
     * @param aText is the text to be drawn.
     * @param aPosition is the text position in world coordinates.
     * @param aCursor is the current text position (for multiple text blocks within a single text
     *                object, such as a run of superscript characters)
     * @param aAttrs are the styling attributes of the text, including its rotation
     * @param aMousePos optional parameter for highlighting urls in text
     * @param aActiveUrl optional [out] parameter for returning highlighted url
     */
    void Draw(MINI::GAL* aGal, const std::string& aText, const VECTOR2I& aPosition,
               const VECTOR2I& aCursor, const TEXT_ATTRIBUTES& aAttributes,
               const METRICS& aFontMetrics, std::optional<VECTOR2I> aMousePos = std::nullopt, std::string* aActiveUrl = nullptr) const;

    void Draw(MINI::GAL* aGal, const std::string& aText, const VECTOR2I& aPosition,
               const TEXT_ATTRIBUTES& aAttributes, const METRICS& aFontMetrics, std::optional<VECTOR2I> aMousePos = std::nullopt,
              std::string* aActiveUrl = nullptr) const
    {
        Draw( aGal, aText, aPosition, VECTOR2I( 0, 0 ), aAttributes, aFontMetrics, aMousePos, aActiveUrl );
    }

    /**
     * Compute the boundary limits of aText (the bounding box of all shapes).
     *
     * @return a VECTOR2I giving the width and height of text.
     */
    VECTOR2I StringBoundaryLimits(const std::string& aText, const VECTOR2I& aSize, int aThickness,
                                   bool aBold, bool aItalic, const METRICS& aFontMetrics ) const;

    /**
     * Insert \n characters into text to ensure that no lines are wider than \a aColumnWidth.
     *
     * This is a highly simplified line-breaker.  KiCad is an EDA tool, not a word processor.
     *
     * -# It breaks only on spaces.  If you type a word wider than the column width then you get
     *    overflow.
     * -# It treats runs of formatted text (superscript, subscript, overbar) as single words.
     * -# It does not perform justification.
     *
     * The results of the linebreaking are the addition of \n in the text.  It is presumed that this
     * function is called on m_shownText (or equivalent) rather than the original source text.
     */
    void LinebreakText(std::string& aText, int aColumnWidth, const VECTOR2I& aGlyphSize,
                        int aThickness, bool aBold, bool aItalic ) const;

    /**
     * Compute the distance (interline) between 2 lines of text (for multiline texts).  This is
     * the distance between baselines, not the space between line bounding boxes.
     */
    virtual double GetInterline( double aGlyphHeight, const METRICS& aFontMetrics ) const = 0;

    /**
     * Convert text string to an array of GLYPHs.
     *
     * @param aBBox pointer to a BOX2I that will set to the bounding box, or nullptr
     * @param aGlyphs storage for the returned GLYPHs
     * @param aText text to convert to polygon/polyline
     * @param aSize is the cap-height and em-width of the text
     * @param aPosition position of text (cursor position before this text)
     * @param aAngle text angle
     * @param aMirror is true if text should be drawn mirrored, false otherwise.
     * @param aOrigin is the point around which the text should be rotated, mirrored, etc.
     * @param aTextStyle text style flags
     * @return text cursor position after this text
     */
    virtual VECTOR2I GetTextAsGlyphs( BOX2I* aBBox, std::vector<std::unique_ptr<GLYPH>>* aGlyphs,
                                     const std::string& aText, const VECTOR2I& aSize,
                                      const VECTOR2I& aPosition, const EDA_ANGLE& aAngle,
                                      bool aMirror, const VECTOR2I& aOrigin,
                                      TEXT_STYLE_FLAGS aTextStyle ) const = 0;

protected:
    /**
     * Return number of lines for a given text.
     *
     * @param aText is the text to be checked.
     * @return unsigned - The number of lines in aText.
     */
    inline unsigned linesCount(const std::string& aText) const
    {
        if( aText.empty() )
            return 0; // std::count does not work well with empty strings
        else
            // aText.end() - 1 is to skip a newline character that is potentially at the end
            return std::count( aText.begin(), aText.end() - 1, '\n' ) + 1;
    }

    /**
     * Draw a single line of text. Multiline texts should be split before using the
     * function.
     *
     * @param aGal is a pointer to the graphics abstraction layer, or nullptr (nothing is drawn)
     * @param aBBox is an optional pointer to be filled with the bounding box.
     * @param aText is the text to be drawn.
     * @param aPosition is text position.
     * @param aSize is the cap-height and em-width of the text
     * @param aAngle is text angle.
     * @param aMirror is true if text should be drawn mirrored, false otherwise.
     * @param aOrigin is the point around which the text should be rotated, mirrored, etc.
     * @param aItalic draw the text in italic
     * @param aUnderline draw the text in underline
     * @param aHover draw the text in hyperlink hover mode (nominally blue + underline)
     * @return new cursor position in non-rotated, non-mirrored coordinates
     */
    void drawSingleLineText(MINI::GAL* aGal, BOX2I* aBoundingBox, const std::string& aText,
                             const VECTOR2I& aPosition, const VECTOR2I& aSize,
                             const EDA_ANGLE& aAngle, bool aMirror, const VECTOR2I& aOrigin,
                             bool aItalic, bool aUnderline, bool aHover, const METRICS& aFontMetrics,
                            std::optional<VECTOR2I> aMousePos, std::string* aActiveUrl) const;

    /**
     * Compute the bounding box for a single line of text.
     *
     * Multiline texts should be split before using the function.
     *
     * @param aBBox is an optional pointer to be filled with the bounding box.
     * @param aText is the text to be drawn.
     * @param aPosition is text position.
     * @param aSize is the cap-height and em-width of the text.
     * @return new cursor position
     */
    VECTOR2I boundingBoxSingleLine(BOX2I* aBBox, const std::string& aText, const VECTOR2I& aPosition,
                                    const VECTOR2I& aSize, bool aItalic,
                                    const METRICS& aFontMetrics ) const;

    void getLinePositions(const std::string& aText, const VECTOR2I& aPosition, std::vector<std::string>& aTextLines,
                          std::vector<VECTOR2I>& aPositions,
                           std::vector<VECTOR2I>& aExtents, const TEXT_ATTRIBUTES& aAttrs,
                           const METRICS& aFontMetrics ) const;

    VECTOR2I drawMarkup( BOX2I* aBoundingBox, std::vector<std::unique_ptr<GLYPH>>* aGlyphs, const std::string& aText,
                        const VECTOR2I& aPosition,
                         const VECTOR2I& aSize, const EDA_ANGLE& aAngle, bool aMirror,
                         const VECTOR2I& aOrigin, TEXT_STYLE_FLAGS aTextStyle,
                         const METRICS& aFontMetrics, std::optional<VECTOR2I> aMousePos = std::nullopt, std::string* aActiveUrl = nullptr) const;

    void wordbreakMarkup(std::vector<std::pair<std::string, int>>* aWords, const std::string& aText,
                          const VECTOR2I& aSize, TEXT_STYLE_FLAGS aTextStyle ) const;

private:
    static FONT* getDefaultFont();

protected:
    std::string m_fontName;     ///< Font name
    std::string m_fontFileName; ///< Font file name

private:
    static FONT* s_defaultFont;

    static std::map<std::tuple<std::string, bool, bool, bool>, FONT*> s_fontMap;
};

} //namespace KIFONT


inline std::ostream& operator<<(std::ostream& os, const KIFONT::FONT& aFont)
{
    os << "[Font \"" << aFont.GetName() << "\"" << ( aFont.IsStroke() ? " stroke" : "" )
       << ( aFont.IsOutline() ? " outline" : "" ) << ( aFont.IsBold() ? " bold" : "" )
       << ( aFont.IsItalic() ? " italic" : "" ) << "]";
    return os;
}


inline std::ostream& operator<<(std::ostream& os, const KIFONT::FONT* aFont)
{
    os << *aFont;
    return os;
}

#endif // FONT_H_
