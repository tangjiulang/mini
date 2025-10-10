#pragma once

#include <optional>

#include <seg.hxx>
#include <box2.hxx>

/*
 * A geometric half-line of infinite length, starting at a given point and extending infinitely.
 * A.k.a. a ray.
 *
 * In terms of geometric ops, a SEG would probably do in most cases, as it
 * has the same definition, but a separate class is more explicit and also
 * allows compile-time reasoning about the meaning of the object through
 * the type system.
 */
class HALF_LINE
{
public:
    /**
     * Construct a ray from a segment - the ray will start at the segment's A point and
     * extend infinitely in the direction of the segment, passing through its B point.
     */
    HALF_LINE( const SEG& aSeg ) : m_seg( aSeg ) {}

    HALF_LINE( const VECTOR2I& aStart, const VECTOR2I& aOtherContainedPoint ) :
            m_seg( aStart, aOtherContainedPoint )
    {
    }

    /**
     * Get the start point of the ray.
     */
    const VECTOR2I& GetStart() const { return m_seg.A; }

    /**
     * Get one (of the infinite number) of points that the ray passes through.
     */
    const VECTOR2I& GetContainedPoint() const { return m_seg.B; }

    bool Contains( const VECTOR2I& aPoint ) const;

    OPT_VECTOR2I Intersect( const SEG& aSeg ) const;

    OPT_VECTOR2I Intersect( const HALF_LINE& aOther ) const;

    /**
     * Get the nearest point on the ray to the given point.
     *
     * This will be the start point of the ray for half the 2D plane.
     */
    VECTOR2I NearestPoint( const VECTOR2I& aPoint ) const;

    /**
     * Based on the ray being identically defined. TODO: this is not geoemetrical equality?!
     */
    bool operator==( const HALF_LINE& aOther ) const { return m_seg == aOther.m_seg; }

    /**
     * Gets the (one of the infinite number of) segments that the ray passes through.
     *
     * The segment's A point is the start of the ray, and the B point is on the ray.
     */
    const SEG& GetContainedSeg() const { return m_seg; }

private:
    /// Internally, we can represent a just a segment that the ray passes through
    SEG m_seg;
};


std::optional<SEG> ClipHalfLineToBox( const HALF_LINE& aRay, const BOX2I& aBox );