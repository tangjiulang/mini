#ifndef __SHAPE_SEGMENT_H
#define __SHAPE_SEGMENT_H

#include <seg.hxx>
#include <shape.hxx>
#include <box2.hxx>
#include <vector2d.hxx>
#include <trigo.hxx>

#include <algorithm>

class SHAPE_SEGMENT : public SHAPE
{
public:
    SHAPE_SEGMENT() :
        SHAPE( SH_SEGMENT ),
        m_width( 0 )
    {};

    SHAPE_SEGMENT( const VECTOR2I& aA, const VECTOR2I& aB, int aWidth = 0 ) :
        SHAPE( SH_SEGMENT ),
        m_seg( aA, aB ),
        m_width( aWidth )
    {};

    SHAPE_SEGMENT( const SEG& aSeg, int aWidth = 0 ) :
        SHAPE( SH_SEGMENT ),
        m_seg( aSeg ),
        m_width( aWidth )
    {};

    ~SHAPE_SEGMENT() {};

    SHAPE* Clone() const override
    {
        return new SHAPE_SEGMENT( m_seg, m_width );
    }

    const BOX2I BBox( int aClearance = 0 ) const override
    {
        return BOX2I( m_seg.A, m_seg.B - m_seg.A ).Inflate( aClearance + ( m_width + 1 ) / 2 );
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

    bool Collide( const SEG& aSeg, int aClearance = 0, int* aActual = nullptr,
                  VECTOR2I* aLocation = nullptr ) const override
    {
        if( aSeg.A == aSeg.B )
            return Collide( aSeg.A, aClearance, aActual, aLocation );

        int min_dist = ( m_width + 1 ) / 2 + aClearance;
        ecoord dist_sq = m_seg.SquaredDistance( aSeg );

        if( dist_sq == 0 || dist_sq < SEG::Square( min_dist ) )
        {
            if( aLocation )
                *aLocation = m_seg.NearestPoint( aSeg );

            if( aActual )
                *aActual = std::max( 0, (int) sqrt( dist_sq ) - ( m_width + 1 ) / 2 );

            return true;
        }

        return false;
    }

    bool Collide( const VECTOR2I& aP, int aClearance = 0, int* aActual = nullptr,
                  VECTOR2I* aLocation = nullptr ) const override
    {
        int min_dist = ( m_width + 1 ) / 2 + aClearance;
        ecoord dist_sq = m_seg.SquaredDistance( aP );

        if( dist_sq == 0 || dist_sq < SEG::Square( min_dist ) )
        {
            if( aLocation )
                *aLocation = m_seg.NearestPoint( aP );

            if( aActual )
                *aActual = std::max( 0, (int) sqrt( dist_sq ) - ( m_width + 1 ) / 2 );

            return true;
        }

        return false;
    }

    void SetSeg( const SEG& aSeg )
    {
        m_seg = aSeg;
    }

    const SEG& GetSeg() const
    {
        return m_seg;
    }

    void SetWidth( int aWidth )
    {
        m_width = aWidth;
    }

    int GetWidth() const
    {
        return m_width;
    }

    bool IsSolid() const override
    {
        return true;
    }

    void Rotate( const EDA_ANGLE& aAngle, const VECTOR2I& aCenter = { 0, 0 } ) override
    {
        RotatePoint( m_seg.A, aCenter, aAngle );
        RotatePoint( m_seg.B, aCenter, aAngle );
    }

    void Move( const VECTOR2I& aVector ) override
    {
        m_seg.A += aVector;
        m_seg.B += aVector;
    }

    bool Is45Degree( EDA_ANGLE aTollerance = EDA_ANGLE( 1.0, DEGREES_T ) ) const;

    virtual const std::string Format( bool aCplusPlus = true ) const override;

    void TransformToPolygon( SHAPE_POLY_SET& aBuffer, int aError,
                             ERROR_LOC aErrorLoc ) const override;

private:
    SEG m_seg;
    int m_width;
};

#endif
