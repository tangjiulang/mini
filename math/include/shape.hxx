#ifndef __SHAPE_H
#define __SHAPE_H

#include <sstream>
#include <vector>
#include "seg.hxx"
#include "eda_angle.hxx"
#include "approximation.hxx"
#include "vector2d.hxx"
#include "box2.hxx"

class SHAPE_LINE_CHAIN;
class SHAPE_POLY_SET;

/**
 * Lists all supported shapes.
 */

enum SHAPE_TYPE
{
    SH_RECT = 0,         ///< axis-aligned rectangle
    SH_SEGMENT,          ///< line segment
    SH_LINE_CHAIN,       ///< line chain (polyline)
    SH_CIRCLE,           ///< circle
    SH_SIMPLE,           ///< simple polygon
    SH_POLY_SET,         ///< set of polygons (with holes, etc.)
    SH_COMPOUND,         ///< compound shape, consisting of multiple simple shapes
    SH_ARC,              ///< circular arc
    SH_NULL,             ///< empty shape (no shape...),
    SH_POLY_SET_TRIANGLE ///< a single triangle belonging to a POLY_SET triangulation
};

static inline std::string SHAPE_TYPE_asString( SHAPE_TYPE a )
{
    switch( a )
    {
    case SH_RECT:              return "SH_RECT";
    case SH_SEGMENT:           return "SH_SEGMENT";
    case SH_LINE_CHAIN:        return "SH_LINE_CHAIN";
    case SH_CIRCLE:            return "SH_CIRCLE";
    case SH_SIMPLE:            return "SH_SIMPLE";
    case SH_POLY_SET:          return "SH_POLY_SET";
    case SH_COMPOUND:          return "SH_COMPOUND";
    case SH_ARC:               return "SH_ARC";
    case SH_NULL:              return "SH_NULL";
    case SH_POLY_SET_TRIANGLE: return "SH_POLY_SET_TRIANGLE";
    }

    return "";  // Just to quiet GCC.
}

class SHAPE;

class SHAPE_BASE
{
public:
    /**
     * Create an empty shape of type aType
     */
    SHAPE_BASE( SHAPE_TYPE aType ) :
        m_type( aType )
    {}

    virtual ~SHAPE_BASE()
    {}

    /**
     * Return the type of the shape.
     *
     * @retval the type
     */
    SHAPE_TYPE Type() const
    {
        return m_type;
    }

    std::string TypeName() const
    {
        return SHAPE_TYPE_asString( m_type );
    }

    virtual bool HasIndexableSubshapes() const
    {
        return false;
    }

    virtual size_t GetIndexableSubshapeCount() const { return 0; }

    virtual void GetIndexableSubshapes( std::vector<const SHAPE*>& aSubshapes ) const { }

protected:
    ///< type of our shape
    SHAPE_TYPE m_type;
};

/**
 * An abstract shape on 2D plane.
 */
class SHAPE : public SHAPE_BASE
{
public:
    /**
     * This is the minimum precision for all the points in a shape.
     */
    static const int MIN_PRECISION_IU = 4;

    /**
     * Create an empty shape of type \a aType.
     */
    SHAPE( SHAPE_TYPE aType ) :
        SHAPE_BASE( aType )
    {}

    virtual ~SHAPE()
    {}

    /**
     * Return a dynamically allocated copy of the shape.
     *
     * @retval copy of the shape
     */
    virtual SHAPE* Clone() const
    {
        assert( false );
        return nullptr;
    };

    /**
     * Return the actual minimum distance between two shapes
     *
     * @retval distance in IU
     */
    int GetClearance( const SHAPE* aOther ) const;

    /**
     * Return true if the shape is a null shape.
     *
     * @retval true if null :-)
     */
    bool IsNull() const
    {
        return m_type == SH_NULL;
    }

    /**
     * Check if the boundary of shape (this) lies closer to the point \a aP than \a aClearance,
     * indicating a collision.
     *
     * @param aActual [out] an optional pointer to an int to store the actual distance in the
     *                event of a collision.
     * @param aLocation [out] an option pointer to a point to store a nearby location in the
     *                  event of a collision.
     * @return true, if there is a collision.
     */
    virtual bool Collide( const VECTOR2I& aP, int aClearance = 0, int* aActual = nullptr,
                          VECTOR2I* aLocation = nullptr ) const
    {
        return Collide( SEG( aP, aP ), aClearance, aActual, aLocation );
    }

    /**
     * Check if the boundary of shape (this) lies closer to the shape \a aShape than \a aClearance,
     * indicating a collision.
     *
     * @param aShape shape to check collision against
     * @param aClearance minimum clearance
     * @param aMTV [out] minimum translation vector
     * @param aActual [out] an optional pointer to an int to store the actual distance in the
     *                event of a collision.
     * @param aLocation [out] an option pointer to a point to store a nearby location in the
     *                  event of a collision.
     * @return true, if there is a collision.
     */
    virtual bool Collide( const SHAPE* aShape, int aClearance, VECTOR2I* aMTV ) const;

    virtual bool Collide( const SHAPE* aShape, int aClearance = 0, int* aActual = nullptr,
                          VECTOR2I* aLocation = nullptr ) const;

    /**
     * Check if the boundary of shape (this) lies closer to the segment \a aSeg than \a aClearance,
     * indicating a collision.
     *
     * @param aActual [out] an optional pointer to an int to be updated with the actual distance
     *                int the event of a collision.
     * @param aLocation [out] an option pointer to a point to store a nearby location in the
     *                  event of a collision.
     * @return true, if there is a collision.
     */
    virtual bool Collide( const SEG& aSeg, int aClearance = 0, int* aActual = nullptr,
                          VECTOR2I* aLocation = nullptr ) const = 0;

    /**
     * Compute a bounding box of the shape, with a margin of \a aClearance a collision.
     *
     * @param aClearance how much the bounding box is expanded wrs to the minimum enclosing
     *                   rectangle for the shape.
     * @return the bounding box.
     */
    virtual const BOX2I BBox( int aClearance = 0 ) const = 0;

    /**
     * Compute a center-of-mass of the shape.
     *
     * @return the center-of-mass point
     */
    virtual VECTOR2I Centre() const
    {
        return BBox( 0 ).Centre(); // if nothing better is available....
    }

    /**
     * Returns the minimum distance from a given point to this shape.
     * Always returns zero if the point is inside a closed shape and aOutlineOnly is false.
     *
     * @param aP is the point to test
     * @return the distance from the shape to aP
     */
    virtual int Distance( const VECTOR2I& aP ) const;

    /**
     * @see SHAPE::Distance
     */
    virtual SEG::ecoord SquaredDistance( const VECTOR2I& aP, bool aOutlineOnly = false ) const;

    /**
     * Return the two points that mark the closest distance between this shape and \a aOther.
     * If the shapes are overlapping, the points will be the same.
     *
     * @param aOther the other shape to compare with
     * @param aPtThis [out] the point on this shape closest to \a aOther
     * @param aPtOther [out] the point on \a aOther closest to this shape
     * @return true if the points were found
     */
    bool NearestPoints( const SHAPE* aOther, VECTOR2I& aPtThis, VECTOR2I& aPtOther ) const;

    /**
     * Check if point \a aP lies inside a closed shape.  Always returns false if this shape is not closed.
     *
     * @param aPt point to check
     * @param aUseBBoxCache gives better performance if the bounding box caches have been
     *                      generated.
     * @return true if the point is inside the shape (edge is not treated as being inside).
     */
    virtual bool PointInside( const VECTOR2I& aPt, int aAccuracy = 0, bool aUseBBoxCache = false ) const;

    /**
     * Fills a SHAPE_POLY_SET with a polygon representation of this shape.
     * @param aBuffer [out] will be filled with the polygonal representation of this shape.
     * @param aError controls the maximum allowed deviation when converting rounded shapes to segments
     * @param aErrorLoc controls where the error is placed when approximating rounded shapes
     */
    virtual void TransformToPolygon( SHAPE_POLY_SET& aBuffer, int aError, ERROR_LOC aErrorLoc ) const = 0;

    /**
     * @param aCenter is the rotation center.
     * @param aAngle rotation angle.
     */
    virtual void Rotate( const EDA_ANGLE& aAngle, const VECTOR2I& aCenter = { 0, 0 } ) = 0;

    virtual void Move( const VECTOR2I& aVector ) = 0;

    virtual bool IsSolid() const = 0;

    virtual bool Parse( std::stringstream& aStream );

    virtual const std::string Format( bool aCplusPlus = true ) const;

protected:
    typedef VECTOR2I::extended_type ecoord;
};


class SHAPE_LINE_CHAIN_BASE : public SHAPE
{
public:
    SHAPE_LINE_CHAIN_BASE( SHAPE_TYPE aType ) :
        SHAPE( aType )
    {
    }

    virtual ~SHAPE_LINE_CHAIN_BASE()
    {
    }

    /**
     * Check if point \a aP lies closer to us than \a aClearance.
     *
     * @param aP the point to check for collisions with
     * @param aClearance minimum distance that does not qualify as a collision.
     * @param aActual an optional pointer to an int to store the actual distance in the event
     *                of a collision.
     * @return true, when a collision has been found
     */
    virtual bool Collide( const VECTOR2I& aP, int aClearance = 0, int* aActual = nullptr,
                          VECTOR2I* aLocation = nullptr ) const override;

    /**
     * Check if segment \a aSeg lies closer to us than \a aClearance.
     *
     * @param aSeg the segment to check for collisions with
     * @param aClearance minimum distance that does not qualify as a collision.
     * @param aActual an optional pointer to an int to store the actual distance in the event
     *                of a collision.
     * @return true, when a collision has been found
     */

    virtual bool Collide( const SEG& aSeg, int aClearance = 0, int* aActual = nullptr,
                          VECTOR2I* aLocation = nullptr ) const override;

    SEG::ecoord SquaredDistance( const VECTOR2I& aP, bool aOutlineOnly = false ) const override;

    bool PointInside( const VECTOR2I& aPt, int aAccuracy = 0, bool aUseBBoxCache = false ) const override;

    /**
     * Check if point \a aP lies on an edge or vertex of the line chain.
     *
     * @param aP point to check
     * @return true if the point lies on the edge.
     */
    bool PointOnEdge( const VECTOR2I& aP, int aAccuracy = 0 ) const;

    /**
     * Check if point \a aP lies on an edge or vertex of the line chain.
     *
     * @param aP point to check
     * @return index of the first edge containing the point, otherwise negative
     */
    int EdgeContainingPoint( const VECTOR2I& aP, int aAccuracy = 0 ) const;

    virtual const VECTOR2I GetPoint( int aIndex ) const   = 0;
    virtual const SEG      GetSegment( int aIndex ) const = 0;
    virtual size_t         GetPointCount() const          = 0;
    virtual size_t         GetSegmentCount() const        = 0;
    virtual bool IsClosed() const = 0;

    virtual BOX2I* GetCachedBBox() const { return nullptr; }

    void TransformToPolygon( SHAPE_POLY_SET& aBuffer, int aError,
                             ERROR_LOC aErrorLoc ) const override
    {}
};

#endif // __SHAPE_H
