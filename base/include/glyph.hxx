#ifndef GLYPH_H
#define GLYPH_H

#include <memory>
#include <box2.hxx>
#include <shape_poly_set.hxx>
#include <eda_angle.hxx>

#if defined( _MSC_VER )
#pragma warning( push )
#pragma warning( disable : 4275 )
#endif

namespace KIFONT
{


class GLYPH
{
public:
    virtual ~GLYPH()
    {}

    virtual bool IsOutline() const { return false; }
    virtual bool IsStroke() const  { return false; }

    virtual BOX2D BoundingBox() = 0;

    bool IsHover() const { return m_isHover; }
    void SetIsHover( bool aIsHover ) { m_isHover = aIsHover; }

private:
    bool m_isHover = false;
};


class OUTLINE_GLYPH : public GLYPH, public SHAPE_POLY_SET
{
public:
    OUTLINE_GLYPH() :
            SHAPE_POLY_SET()
    {}

    OUTLINE_GLYPH( const OUTLINE_GLYPH& aGlyph ) :
            SHAPE_POLY_SET( aGlyph )
    {}

    OUTLINE_GLYPH( const SHAPE_POLY_SET& aPoly ) :
            SHAPE_POLY_SET( aPoly )
    {}

    bool IsOutline() const override { return true; }

    BOX2D BoundingBox() override;

    void Triangulate( std::function<void( const VECTOR2I& aPt1,
                                          const VECTOR2I& aPt2,
                                          const VECTOR2I& aPt3 )> aCallback ) const;

    void CacheTriangulation( bool aPartition = true, bool aSimplify = false ) override;

    /**
     * @return a set of triangulated polygons from the glyph.  CacheTriangulation() will use this
     * data as hint data the next time around.
     */
    std::vector<std::unique_ptr<SHAPE_POLY_SET::TRIANGULATED_POLYGON>> GetTriangulationData() const;

    /**
     * Cache the triangulation for the glyph from a known set of triangle indexes.
     * (See GetTriangulationData() above for more info.)
     */
    void CacheTriangulation( std::vector<std::unique_ptr<SHAPE_POLY_SET::TRIANGULATED_POLYGON>>& aHintData );
};


class STROKE_GLYPH : public GLYPH, public std::vector<std::vector<VECTOR2D>>
{
public:
    STROKE_GLYPH()
    {}

    STROKE_GLYPH( const STROKE_GLYPH& aGlyph );

    bool IsStroke() const override { return true; }

    void AddPoint( const VECTOR2D& aPoint );
    void RaisePen();
    void Finalize();

    BOX2D BoundingBox() override { return m_boundingBox; }
    void SetBoundingBox( const BOX2D& bbox ) { m_boundingBox = bbox; }

    std::unique_ptr<GLYPH> Transform( const VECTOR2D& aGlyphSize,  const VECTOR2I& aOffset,
                                      double aTilt, const EDA_ANGLE& aAngle, bool aMirror,
                                      const VECTOR2I& aOrigin  );

    void Move( const VECTOR2I& aOffset );

private:
    bool  m_penIsDown = false;
    BOX2D m_boundingBox;
};



} // namespace KIFONT

#if defined( _MSC_VER )
#pragma warning( pop )
#endif

#endif  // GLYPH_H
