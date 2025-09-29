#ifndef __SHAPE_RECT_H
#define __SHAPE_RECT_H

#include <seg.hxx>
#include <shape.hxx>
#include <shape_line_chain.hxx>
#include <box2.hxx>
#include <vector2d.hxx>
#include <trigo.hxx>

class SHAPE_RECT : public SHAPE
{
public:
    /**
     * Create an empty (0-sized) rectangle.
     */
    SHAPE_RECT() :
        SHAPE( SH_RECT ),
        m_w( 0 ),
        m_h( 0 )
    {}

    /**
     * Create a rectangle defined by a BOX2.
     */
    SHAPE_RECT( const BOX2I& aBox ) :
        SHAPE( SH_RECT ),
        m_p0( aBox.GetPosition() ),
        m_w( aBox.GetWidth() ),
        m_h( aBox.GetHeight() )
    {}

    /**
     * Create a rectangle defined by top-left corner (aX0, aY0), width aW and height aH.
     */
    SHAPE_RECT( int aX0, int aY0, int aW, int aH ) :
        SHAPE( SH_RECT ),
        m_p0( aX0, aY0 ),
        m_w( aW ),
        m_h( aH )
    {}

    /**
     * Create a rectangle defined by top-left corner aP0, width aW and height aH.
     */
    SHAPE_RECT( const VECTOR2I& aP0, int aW, int aH ) :
        SHAPE( SH_RECT ),
        m_p0( aP0 ),
        m_w( aW ),
        m_h( aH )
    {}

    /**
     * Create by two corners.
     */
    SHAPE_RECT( const VECTOR2I& aP0, const VECTOR2I& aP1 ) :
        SHAPE( SH_RECT ),
        m_p0( aP0 ),
        m_w( aP1.x - aP0.x ),
        m_h( aP1.y - aP0.y )
    {}

    SHAPE_RECT( const SHAPE_RECT& aOther ) :
        SHAPE( SH_RECT ),
        m_p0( aOther.m_p0 ),
        m_w( aOther.m_w ),
        m_h( aOther.m_h )
    {};

    SHAPE* Clone() const override
    {
        return new SHAPE_RECT( *this );
    }

    /// @copydoc SHAPE::BBox()
    const BOX2I BBox( int aClearance = 0 ) const override
    {
        BOX2I bbox( VECTOR2I( m_p0.x - aClearance,  m_p0.y - aClearance ),
                    VECTOR2I( m_w + 2 * aClearance, m_h + 2 * aClearance ) );
        return bbox;
    }

    /**
     * Return a rectangle that is larger by aOffset in all directions,
     * but still centered on the original rectangle.
     */
    SHAPE_RECT GetInflated( int aOffset ) const
    {
        return SHAPE_RECT{
            m_p0 - VECTOR2I( aOffset, aOffset ),
            m_w + 2 * aOffset,
            m_h + 2 * aOffset,
        };
    }

    /**
     * Return length of the diagonal of the rectangle.
     *
     * @return diagonal length
     */
    int Diagonal() const
    {
        return VECTOR2I( m_w, m_h ).EuclideanNorm();
    }

    int MajorDimension() const
    {
        return std::max( m_w, m_h );
    }

    int MinorDimension() const
    {
        return std::min( m_w, m_h );
    }

    bool Collide( const SHAPE* aShape, int aClearance, VECTOR2I* aMTV ) const override
    {
        return SHAPE::Collide( aShape, aClearance, aMTV );
    }

    bool Collide( const SHAPE* aShape, int aClearance = 0, int* aActual = nullptr,
                  VECTOR2I* aLocation = nullptr ) const override
    {
        return SHAPE::Collide( aShape, aClearance, aActual, aLocation );
    }

    /// @copydoc SHAPE::Collide()
    bool Collide( const SEG& aSeg, int aClearance = 0, int* aActual = nullptr,
                  VECTOR2I* aLocation = nullptr ) const override;

    /**
     * @return the top left corner of the rectangle.
     */
    const VECTOR2I& GetPosition() const
    {
        return m_p0;
    }

    /**
     * @return the size of the rectangle.
     */
    const VECTOR2I GetSize() const
    {
        return VECTOR2I( m_w, m_h );
    }

    /**
     * @return the width of the rectangle.
     */
     int GetWidth() const
     {
         return m_w;
     }

    /**
     * @return the height of the rectangle.
     */
    int GetHeight() const
    {
        return m_h;
    }

    void Move( const VECTOR2I& aVector ) override
    {
        m_p0 += aVector;
    }

    /**
     * This function has limited utility for SHAPE_RECT as non-cartesian rotations will distort
     * the rectangle.  If you might need to handle non-90° rotations then the SHAPE_RECT should
     * first be converted to a SHAPE_SIMPLE which can then be free-rotated.
     */
    void Rotate( const EDA_ANGLE& aAngle, const VECTOR2I& aCenter = { 0, 0 } ) override
    {
        RotatePoint( m_p0, aCenter, aAngle );

        if( abs( aAngle.Sin() ) == 1 )
            std::swap( m_h, m_w );
    }

    bool IsSolid() const override
    {
        return true;
    }

    const SHAPE_LINE_CHAIN Outline() const
    {
        SHAPE_LINE_CHAIN rv;
        rv.Append( m_p0 );
        rv.Append( m_p0.x, m_p0.y + m_h );
        rv.Append( m_p0.x + m_w, m_p0.y + m_h );
        rv.Append( m_p0.x + m_w, m_p0.y );
        rv.Append( m_p0 );
        rv.SetClosed( true );
        return rv;
    }

    virtual const std::string Format( bool aCplusPlus = true ) const override;

    void TransformToPolygon( SHAPE_POLY_SET& aBuffer, int aError,
                             ERROR_LOC aErrorLoc ) const override;

private:
    VECTOR2I m_p0;      ///< Top-left corner
    int      m_w;       ///< Width
    int      m_h;       ///< Height
};

#endif // __SHAPE_RECT_H
