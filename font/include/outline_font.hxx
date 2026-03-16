#ifndef OUTLINE_FONT_H_
#define OUTLINE_FONT_H_

#include <gal/include/gal.hxx>
#include <shape_poly_set.hxx>
#ifdef _MSC_VER
#include <ft2build.h>
#else
#include <freetype2/ft2build.h>
#endif
#include FT_FREETYPE_H
#include FT_OUTLINE_H

#include <font.hxx>
#include <glyph.hxx>
#include <outline_decomposer.hxx>

#include <mutex>

namespace KIFONT
{
/**
 * Class OUTLINE_FONT implements outline font drawing.
 */
class OUTLINE_FONT : public FONT
{
public:

    enum class EMBEDDING_PERMISSION
    {
        INSTALLABLE,
        EDITABLE,
        PRINT_PREVIEW_ONLY,
        RESTRICTED,
        INVALID
    };

    OUTLINE_FONT();

    bool IsOutline() const override { return true; }

    bool IsBold() const override
    {
        return m_face && ( m_fakeBold || ( m_face->style_flags & FT_STYLE_FLAG_BOLD ) );
    }

    bool IsItalic() const override
    {
        return m_face && ( m_fakeItal || ( m_face->style_flags & FT_STYLE_FLAG_ITALIC ) );
    }

    // Accessors to distinguish fake vs real style for diagnostics and rendering decisions
    bool IsFakeItalic() const { return m_fakeItal; }
    bool IsFakeBold() const { return m_fakeBold; }

    void SetFakeBold()
    {
        m_fakeBold = true;
    }

    void SetFakeItal()
    {
        m_fakeItal = true;
    }

    const std::string& GetFileName() const { return m_fontFileName; }

    EMBEDDING_PERMISSION GetEmbeddingPermission() const;

    /**
     * Load an outline font. TrueType (.ttf) and OpenType (.otf) are supported.
     *
     * @param aFontFileName is the (platform-specific) fully qualified name of the font file
     */
    static OUTLINE_FONT* LoadFont(const std::string& aFontFileName, bool aBold, bool aItalic,
                                  const std::vector<std::string>* aEmbeddedFiles,
                                   bool aForDrawingSheet );

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

    void GetLinesAsGlyphs(std::vector<std::unique_ptr<GLYPH>>* aGlyphs, const std::string& aText,
                           const VECTOR2I& aPosition, const TEXT_ATTRIBUTES& aAttrs,
                           const METRICS& aFontMetrics ) const;

    const FT_Face& GetFace() const { return m_face; }

#if 0
    void RenderToOpenGLCanvas( KIGFX::OPENGL_FREETYPE& aTarget, const wxString& aString,
                               const VECTOR2D& aSize, const wxPoint& aPosition,
                               const EDA_ANGLE& aAngle, bool aMirror ) const;
#endif

protected:
    FT_Error loadFace(const std::string& aFontFileName, int aFaceIndex);

    BOX2I getBoundingBox( const std::vector<std::unique_ptr<GLYPH>>& aGlyphs ) const;

    VECTOR2I getTextAsGlyphs( BOX2I* aBoundingBox, std::vector<std::unique_ptr<GLYPH>>* aGlyphs,
                             const std::string& aText, const VECTOR2I& aSize,
                              const VECTOR2I& aPosition, const EDA_ANGLE& aAngle, bool aMirror,
                              const VECTOR2I& aOrigin, TEXT_STYLE_FLAGS aTextStyle ) const;

private:
    VECTOR2I getTextAsGlyphsUnlocked( BOX2I* aBoundingBox,
                                      std::vector<std::unique_ptr<GLYPH>>* aGlyphs,
                                     const std::string& aText, const VECTOR2I& aSize,
                                      const VECTOR2I& aPosition, const EDA_ANGLE& aAngle,
                                      bool aMirror, const VECTOR2I& aOrigin,
                                      TEXT_STYLE_FLAGS aTextStyle ) const;

private:
    // FreeType variables

    /**
     * Mutex for freetype access, FT_Library and FT_Face are not thread safe
     */
    static std::mutex m_freeTypeMutex;
    static FT_Library m_freeType;
    FT_Face           m_face;

    const int         m_faceSize;
    bool              m_fakeBold;
    bool              m_fakeItal;

    bool              m_forDrawingSheet;

    // cache for glyphs converted to straight segments
    // key is glyph index (FT_GlyphSlot field glyph_index)
    std::map<unsigned int, std::vector<std::vector<VECTOR2D>>> m_contourCache;

    // The height of the KiCad stroke font is the distance between stroke endpoints for a vertical
    // line of cap-height.  So the cap-height of the font is actually stroke-width taller than its
    // height.
    // Outline fonts are normally scaled on full-height (including ascenders and descenders), so we
    // need to compensate to keep them from being much smaller than their stroked counterparts.
    static constexpr double m_outlineFontSizeCompensation = 1.4;

    // FT_Set_Char_Size() gets character width and height specified in 1/64ths of a point
    static constexpr int m_charSizeScaler = 64;

    // The KiCad stroke font uses a subscript/superscript size ratio of 0.7.  This ratio is also
    // commonly used in LaTeX, but fonts with designed-in subscript and superscript glyphs are more
    // likely to use 0.58.
    // For auto-generated subscript and superscript glyphs in outline fonts we split the difference
    // with 0.64.
    static constexpr double m_subscriptSuperscriptSize = 0.64;

    static constexpr double m_underlineOffsetScaler = -0.16;

    int faceSize( int aSize ) const
    {
        return aSize * m_charSizeScaler * m_outlineFontSizeCompensation;
    };

    int faceSize() const { return faceSize( m_faceSize ); }

    // also for superscripts
    int subscriptSize( int aSize ) const
    {
        return KiROUND( faceSize( aSize ) * m_subscriptSuperscriptSize );
    }
    int subscriptSize() const { return subscriptSize( m_faceSize ); }

    static constexpr double m_subscriptVerticalOffset   = -0.25;
    static constexpr double m_superscriptVerticalOffset = 0.45;
};

} //namespace KIFONT

#endif // OUTLINE_FONT_H_
