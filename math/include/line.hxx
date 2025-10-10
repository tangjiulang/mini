#pragma once

#include <seg.hxx>

/*
 * A geometric line of infinite length.
 *
 * In terms of geometric ops, a SEG would probably do as it has the same definition,
 * but a separate class is more explicit and also allows compile-time
 * reasoning about the meaning of the object through the type system.
 */
class LINE
{
public:
    using ecoord = VECTOR2I::extended_type;

    LINE( const SEG& aSeg ) : m_seg( aSeg ) {}

    LINE( const VECTOR2I& aStart, const VECTOR2I& aEnd ) : m_seg( aStart, aEnd ) {}

    bool operator==( const LINE& aOther ) const { return m_seg == aOther.m_seg; }

    /**
     * Gets the (one of the infinite number of) segments that the line passes through.
     */
    const SEG& GetContainedSeg() const { return m_seg; }

    OPT_VECTOR2I Intersect( const SEG& aOther ) const;
    OPT_VECTOR2I Intersect( const LINE& aOther ) const;

    /**
     * Gets the distance from the line to the given point.
     */
    int Distance( const VECTOR2I& aPoint ) const;

    /**
     * Gets the nearest point on the line to the given point.
     */
    VECTOR2I NearestPoint( const VECTOR2I& aPoint ) const;

private:
    /// Internally, we can represent a just a segment that the line passes through
    SEG m_seg;
};