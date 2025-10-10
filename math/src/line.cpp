#include <line.hxx>

OPT_VECTOR2I LINE::Intersect( const SEG& aSeg ) const
{
    // intersect as two lines
    OPT_VECTOR2I intersection = aSeg.Intersect( m_seg, false, true );

    if( intersection )
    {
        // Not parallel.
        // That was two lines, but we need to check if the intersection is on
        // the requested segment
        if( aSeg.Contains( *intersection ) )
        {
            return intersection;
        }
    }
    return std::nullopt;
}

OPT_VECTOR2I LINE::Intersect( const LINE& aOther ) const
{
    // Defer to the SEG implementation
    return aOther.m_seg.Intersect( m_seg, false, true );
}

int LINE::Distance( const VECTOR2I& aPoint ) const
{
    // Just defer to the SEG implementation
    return m_seg.LineDistance( aPoint );
}

VECTOR2I LINE::NearestPoint( const VECTOR2I& aPoint ) const
{
    // Same as the SEG implementation, but without the early return
    // if the point isn't on the segment.

    // Inlined for performance reasons
    VECTOR2L d( m_seg.B.x - m_seg.A.x, m_seg.B.y - m_seg.A.y );
    ecoord   l_squared( d.x * d.x + d.y * d.y );

    if( l_squared == 0 )
        return m_seg.A;

    ecoord t = d.Dot( aPoint - m_seg.A );

    ecoord xp = rescale( t, (ecoord) d.x, l_squared );
    ecoord yp = rescale( t, (ecoord) d.y, l_squared );

    return VECTOR2<ecoord>( m_seg.A.x + xp, m_seg.A.y + yp );
}