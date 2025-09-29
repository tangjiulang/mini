#include <shape_rect.hxx>
#include <shape_poly_set.hxx>

bool SHAPE_RECT::Collide( const SEG& aSeg, int aClearance, int* aActual, VECTOR2I* aLocation ) const
{
    BOX2I bbox( BBox() );

    if( bbox.Contains( aSeg.A ) )
    {
        if( aLocation )
            *aLocation = aSeg.A;

        if( aActual )
            *aActual = 0;

        return true;
    }

    if( bbox.Contains( aSeg.B ) )
    {
        if( aLocation )
            *aLocation = aSeg.B;

        if( aActual )
            *aActual = 0;

        return true;
    }

    VECTOR2I corners[] = { VECTOR2I( m_p0.x, m_p0.y ),
                           VECTOR2I( m_p0.x, m_p0.y + m_h ),
                           VECTOR2I( m_p0.x + m_w, m_p0.y + m_h ),
                           VECTOR2I( m_p0.x + m_w, m_p0.y ),
                           VECTOR2I( m_p0.x, m_p0.y ) };

    SEG::ecoord closest_dist_sq = VECTOR2I::ECOORD_MAX;
    VECTOR2I nearest;

    for( int i = 0; i < 4; i++ )
    {
        SEG side( corners[i], corners[ i + 1] );
        SEG::ecoord dist_sq = side.SquaredDistance( aSeg );

        if( dist_sq < closest_dist_sq )
        {
            if ( aLocation )
            {
                nearest = side.NearestPoint( aSeg );
            }

            closest_dist_sq = dist_sq;
        }
        else if( aLocation && dist_sq == closest_dist_sq )
        {
            VECTOR2I near = side.NearestPoint( aSeg );

            if( ( near - aSeg.A ).SquaredEuclideanNorm()
                < ( nearest - aSeg.A ).SquaredEuclideanNorm() )
            {
                nearest = near;
            }
        }
    }

    if( closest_dist_sq == 0 || closest_dist_sq < SEG::Square( aClearance ) )
    {
        if( aActual )
            *aActual = sqrt( closest_dist_sq );

        if( aLocation )
            *aLocation = nearest;

        return true;
    }

    return false;
}

const std::string SHAPE_RECT::Format( bool aCplusPlus ) const
{
    std::stringstream ss;

    ss << "SHAPE_RECT( ";
    ss << m_p0.x;
    ss << ", ";
    ss << m_p0.y;
    ss << ", ";
    ss << m_w;
    ss << ", ";
    ss << m_h;
    ss << ");";

    return ss.str();
}


void SHAPE_RECT::TransformToPolygon( SHAPE_POLY_SET& aBuffer, int aError,
                                     ERROR_LOC aErrorLoc ) const
{
    int idx = aBuffer.NewOutline();
    SHAPE_LINE_CHAIN& outline = aBuffer.Outline( idx );

    outline.Append( m_p0 );
    outline.Append( { m_p0.x + m_w, m_p0.y } );
    outline.Append( { m_p0.x + m_w, m_p0.y + m_h } );
    outline.Append( { m_p0.x, m_p0.y + m_h } );
    outline.SetClosed( true );
}
