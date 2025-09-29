#ifndef __SHAPE_CIRCLE_H
#define __SHAPE_CIRCLE_H

#include <shape.hxx>
#include <circle.hxx>
#include <box2.hxx>
#include <vector2d.hxx>
#include <trigo.hxx>

#include <algorithm>

class SHAPE_CIRCLE : public SHAPE
{
public:
    SHAPE_CIRCLE() :
        SHAPE( SH_CIRCLE ),
        m_circle()
    {}

    SHAPE_CIRCLE( const VECTOR2I& aCenter, int aRadius ) :
        SHAPE( SH_CIRCLE ),
        m_circle( aCenter, aRadius )
    {}

    SHAPE_CIRCLE( const CIRCLE& aCircle ) :
        SHAPE( SH_CIRCLE ),
        m_circle( aCircle )
    {}

    SHAPE_CIRCLE( const SHAPE_CIRCLE& aOther ) :
        SHAPE( SH_CIRCLE ),
        m_circle( aOther.m_circle )
    {};

    ~SHAPE_CIRCLE()
    {}

    SHAPE* Clone() const override
    {
        return new SHAPE_CIRCLE( *this );
    }

    SHAPE_CIRCLE& operator=( const SHAPE_CIRCLE& ) = default;

    const BOX2I BBox( int aClearance = 0 ) const override
    {
        const VECTOR2I rc( m_circle.Radius + aClearance, m_circle.Radius + aClearance );

        return BOX2I( m_circle.Center - rc, rc * 2 );
    }

    bool Collide( const SEG& aSeg, int aClearance = 0, int* aActual = nullptr,
                  VECTOR2I* aLocation = nullptr ) const override
    {
        int      minDist = aClearance + m_circle.Radius;
        VECTOR2I pn = aSeg.NearestPoint( m_circle.Center );
        ecoord   dist_sq = ( pn - m_circle.Center ).SquaredEuclideanNorm();

        if( dist_sq == 0 || dist_sq < SEG::Square( minDist ) )
        {
            if( aLocation )
            {
                if( std::vector<VECTOR2I> pts = m_circle.Intersect( aSeg );
                    !pts.empty() && dist_sq == 0 )
                {
                    *aLocation = m_circle.Intersect( aSeg )[0];
                }
                else
                {
                    *aLocation = pn;
                }
            }

            if( aActual )
                *aActual = std::max( 0, (int) sqrt( dist_sq ) - m_circle.Radius );

            return true;
        }

        return false;
    }

    void SetRadius( int aRadius )
    {
        m_circle.Radius = aRadius;
    }

    void SetCenter( const VECTOR2I& aCenter )
    {
        m_circle.Center = aCenter;
    }

    int GetRadius() const
    {
        return m_circle.Radius;
    }

    const VECTOR2I GetCenter() const
    {
        return m_circle.Center;
    }

    const CIRCLE GetCircle() const
    {
        return m_circle;
    }

    void Move( const VECTOR2I& aVector ) override
    {
        m_circle.Center += aVector;
    }

    void Rotate( const EDA_ANGLE& aAngle, const VECTOR2I& aCenter = { 0, 0 } ) override
    {
        RotatePoint( m_circle.Center, aCenter, aAngle );
    }

    bool IsSolid() const override
    {
        return true;
    }

    virtual const std::string Format( bool aCplusPlus = true ) const override;

    void TransformToPolygon( SHAPE_POLY_SET& aBuffer, int aError,
                             ERROR_LOC aErrorLoc ) const override;

private:
    CIRCLE m_circle;
};

#endif
