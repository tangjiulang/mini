#ifndef GEOMETRY_POINT_TYPE_H_
#define GEOMETRY_POINT_TYPE_H_

#include <vector2d.hxx>

/**
 * Meanings that can be assigned to a point in pure geometric
 * terms.
 *
 * For example, a circle has a center point and four quadrant points.
 *
 * These can be combined using bitwise OR if a point has multiple meanings.
 */
enum POINT_TYPE
{
    /**
     * No specific point type.
     */
    PT_NONE = 0,
    /**
     * The point is the center of something.
     */
    PT_CENTER = 1 << 0,
    /**
     * The point is at the end of a segment, arc, etc.
     */
    PT_END = 1 << 1,
    /**
     * The point is at the middle of a segment, arc, etc.
     */
    PT_MID = 1 << 2,
    /**
     * The point is on a quadrant of a circle (N, E, S, W points).
     */
    PT_QUADRANT = 1 << 3,
    /**
     * The point is a corner of a polygon, rectangle, etc
     * (you may want to infer PT_END from this)
     */
    PT_CORNER = 1 << 4,
    /**
     * The point is an intersection of two (or more) items.
     */
    PT_INTERSECTION = 1 << 5,
    /**
     * The point is somewhere on another element, but not some specific point.
     * (you can infer this from some other point types)
     */
    PT_ON_ELEMENT = 1 << 6,
};

struct TYPED_POINT2I
{
    VECTOR2I m_point;
    // Bitwise OR of POINT_TYPE values
    int m_types;

    // Clang needs this apparently
    TYPED_POINT2I( const VECTOR2I& aVec, int aTypes ) : m_point( aVec ), m_types( aTypes ) {}

    bool operator==( const TYPED_POINT2I& ) const = default;
};

#endif // GEOMETRY_POINT_TYPE_H_