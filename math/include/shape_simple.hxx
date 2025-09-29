#ifndef __SHAPE_SIMPLE_H
#define __SHAPE_SIMPLE_H

#include "seg.hxx"
#include "shape.hxx"
#include "shape_line_chain.hxx"
#include "box2.hxx"
#include "vector2d.hxx"

/**
 * Represent a simple polygon consisting of a zero-thickness closed chain of
 * connected line segments.
 *
 * Internally the vertices are held in a SHAPE_LINE_CHAIN, please note that
 * there is a "virtual" line segment between the last and first vertex.
 */

class SHAPE_SIMPLE : public SHAPE_LINE_CHAIN_BASE
{
public:
    /**
     * Create an empty polygon.
     */
    SHAPE_SIMPLE() :
        SHAPE_LINE_CHAIN_BASE( SH_SIMPLE )
    {
        m_points.SetClosed( true );
    }

    SHAPE_SIMPLE( const SHAPE_LINE_CHAIN& aPoly ) :
        SHAPE_LINE_CHAIN_BASE( SH_SIMPLE ),
        m_points( aPoly )
    {
        m_points.SetClosed( true );
    }

    SHAPE_SIMPLE( const SHAPE_SIMPLE& aOther ) :
       SHAPE_LINE_CHAIN_BASE( SH_SIMPLE ), m_points( aOther.m_points )
    {}

    SHAPE* Clone() const override
    {
        return new SHAPE_SIMPLE( *this );
    }

    /**
     * Remove all points from the polygon.
     */
    void Clear()
    {
        m_points.Clear();
    }

    /// @copydoc SHAPE::BBox()
    const BOX2I BBox( int aClearance = 0 ) const override
    {
        return m_points.BBox( aClearance );
    }

    /**
     * Return the number of points (vertices) in this polygon.
     *
     * @return number of points.
     */
    int PointCount() const
    {
        return m_points.PointCount();
    }

    /**
     * Return a const reference to a given point in the polygon.
     *
     * Negative indices count from the end of the point list, e.g. -1 means "last point", -2 means
     * "second to last point" and so on.
     *
     * @param aIndex is the index of the point.
     * @return the const reference to the point.
     */
    const VECTOR2I& CPoint( int aIndex ) const
    {
        return m_points.CPoint( aIndex );
    }

    /**
     * Return a given point as a vector with elements of type double.
     *
     * @param aIndex is the index of the point.
     * @return the point with elements of type double.
     */
    const VECTOR2D CDPoint( int aIndex ) const
    {
        const VECTOR2I& v = CPoint( aIndex );
        return VECTOR2D( v.x, v.y );
    }

    /**
     * Return the list of vertices defining this simple polygon.
     *
     * @return the list of vertices defining this simple polygon.
     */
    const SHAPE_LINE_CHAIN& Vertices() const
    {
        return m_points;
    }

    /**
     * Append a new point at the end of the polygon.
     *
     * @param aX is X coordinate of the new point.
     * @param aY is Y coordinate of the new point.
     */
    void Append( int aX, int aY )
    {
        VECTOR2I v( aX, aY );
        Append( v );
    }

    /**
     * Append a new point at the end of the polygon.
     *
     * @param aP is the new point.
     */
    void Append( const VECTOR2I& aP )
    {
        m_points.Append( aP );
    }

    /// @copydoc SHAPE::Collide()
    bool Collide( const SEG& aSeg, int aClearance = 0, int* aActual = nullptr,
                  VECTOR2I* aLocation = nullptr ) const override
    {
        return m_points.Collide( aSeg, aClearance, aActual, aLocation );
    }

    void Rotate( const EDA_ANGLE& aAngle, const VECTOR2I& aCenter = { 0, 0 } ) override
    {
        m_points.Rotate( aAngle, aCenter );
    }

    void Move( const VECTOR2I& aVector ) override
    {
        m_points.Move( aVector );
    }

    bool IsSolid() const override
    {
        return true;
    }

    virtual const VECTOR2I GetPoint( int aIndex ) const override { return m_points.CPoint(aIndex); }
    virtual const SEG GetSegment( int aIndex ) const override { return m_points.CSegment(aIndex); }
    virtual size_t GetPointCount() const override { return m_points.PointCount(); }
    virtual size_t GetSegmentCount() const override { return m_points.SegmentCount(); }

    bool IsClosed() const override
    {
        return true;
    }

    void TransformToPolygon( SHAPE_POLY_SET& aBuffer, int aError,
                             ERROR_LOC aErrorLoc ) const override;

private:
    // vertices
    SHAPE_LINE_CHAIN m_points;
};

#endif // __SHAPE_SIMPLE_H
