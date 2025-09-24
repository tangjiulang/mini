#include "vector2d.hxx"

bool std::less<VECTOR2I>::operator()( const VECTOR2I& aA, const VECTOR2I& aB ) const
{
    if( aA.x == aB.x )
        return aA.y < aB.y;

    return aA.x < aB.x;
}
