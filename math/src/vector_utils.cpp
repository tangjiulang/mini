#include "vector_utils.hxx"

#include <seg.hxx>

using namespace KIGEOM;

bool KIGEOM::SegIsInDirection( const SEG& aSeg, const VECTOR2I& aDirection )
{
    return PointIsInDirection( aSeg.B, aDirection, aSeg.A );
};


bool KIGEOM::PointProjectsOntoSegment( const VECTOR2I& aPoint, const SEG& aSeg )
{
    // SEG::NearestPoint returns the end points if the projection is
    // outside the segment
    const VECTOR2I projected = aSeg.NearestPoint( aPoint );
    return projected != aSeg.A && projected != aSeg.B;
}


double KIGEOM::GetLengthRatioFromStart( const VECTOR2I& aPoint, const SEG& aSeg )
{
    const double length = aSeg.Length();
    const double projected_length = ( aPoint - aSeg.A ).EuclideanNorm();
    return projected_length / length;
}


double KIGEOM::GetProjectedPointLengthRatio( const VECTOR2I& aPoint, const SEG& aSeg )
{
    const VECTOR2I projected = aSeg.NearestPoint( aPoint );

    if( projected == aSeg.A )
        return 0.0;
    if( projected == aSeg.B )
        return 1.0;

    return GetLengthRatioFromStart( projected, aSeg );
}


const VECTOR2I& KIGEOM::GetNearestEndpoint( const SEG& aSeg, const VECTOR2I& aPoint )
{
    const double distToCBStart = aSeg.A.Distance( aPoint );
    const double distToCBEnd = aSeg.B.Distance( aPoint );
    return ( distToCBStart <= distToCBEnd ) ? aSeg.A : aSeg.B;
}

template <typename T>
static constexpr T RoundNearest( T x, T g )
{
    return ( x + ( x < 0 ? -g / 2 : g / 2 ) ) / g * g;
}

template <typename T>
static constexpr T RoundDown( T x, T g )
{
    return ( ( x < 0 ? ( x - g + 1 ) : x ) / g ) * g;
}

template <typename T>
static constexpr T RoundUp( T x, T g )
{
    return ( ( x < 0 ? x : ( x + g - 1 ) ) / g ) * g;
}

VECTOR2I KIGEOM::RoundGrid( const VECTOR2I& aVec, int aGridSize )
{
    return VECTOR2I( RoundNearest( aVec.x, aGridSize ), RoundNearest( aVec.y, aGridSize ) );
}

VECTOR2I KIGEOM::RoundNW( const VECTOR2I& aVec, int aGridSize )
{
    return VECTOR2I( RoundDown( aVec.x, aGridSize ), RoundDown( aVec.y, aGridSize ) );
}

VECTOR2I KIGEOM::RoundSE( const VECTOR2I& aVec, int aGridSize )
{
    return VECTOR2I( RoundUp( aVec.x, aGridSize ), RoundUp( aVec.y, aGridSize ) );
}