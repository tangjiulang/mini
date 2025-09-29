#include <sstream>
#include <string>

#include <shape_compound.hxx>

const std::string SHAPE_COMPOUND::Format( bool aCplusPlus ) const
{
    std::stringstream ss;

    ss << "compound( ";

    for( auto shape : m_shapes )
        ss << shape->Format() << " ";

    return ss.str();
}

SHAPE_COMPOUND::SHAPE_COMPOUND( const std::vector<SHAPE*>& aShapes ) :
         SHAPE( SH_COMPOUND ),
         m_dirty( true ),
         m_shapes( aShapes )
{

}


SHAPE_COMPOUND::SHAPE_COMPOUND( const SHAPE_COMPOUND& aOther )
    : SHAPE( SH_COMPOUND )
{
    for ( const SHAPE* shape : aOther.Shapes() )
        m_shapes.push_back( shape->Clone() );

    m_dirty = true;
}




SHAPE_COMPOUND::~SHAPE_COMPOUND()
{
    for( auto shape : m_shapes )
        delete shape;
}


SHAPE_COMPOUND* SHAPE_COMPOUND::Clone() const
{
    return new SHAPE_COMPOUND( *this );
}


const BOX2I SHAPE_COMPOUND::BBox( int aClearance ) const
{
    BOX2I bb;

    if ( m_shapes.size() < 1 )
        return bb;

    bb = m_shapes[0]->BBox();

    for( size_t i = 1; i < m_shapes.size(); i++ )
        bb.Merge( m_shapes[i]->BBox() );

    return bb;
}

void SHAPE_COMPOUND::Move ( const VECTOR2I& aVector )
{
         for( auto& item : m_shapes )
            item->Move( aVector );
}


int SHAPE_COMPOUND::Distance( const SEG& aSeg ) const
{
    assert(false);
    return 0;   // Make compiler happy
}


void SHAPE_COMPOUND::Rotate( const EDA_ANGLE& aAngle, const VECTOR2I& aCenter )
{
    assert( false );
}


bool SHAPE_COMPOUND::IsSolid() const
{
    return true;
}


bool SHAPE_COMPOUND::Collide( const SEG& aSeg, int aClearance, int* aActual,
                              VECTOR2I* aLocation ) const
{
    int closest_dist = std::numeric_limits<int>::max();
    VECTOR2I nearest;

    for( SHAPE* item : m_shapes )
    {
        int actual = 0;
        VECTOR2I pn;

        if( item->Collide( aSeg, aClearance,
                           aActual || aLocation ? &actual : nullptr,
                           aLocation ? &pn : nullptr ) )
        {
            if( actual < closest_dist )
            {
                nearest = pn;
                closest_dist = actual;

                if( !aLocation && !aActual )
                    break;
            }
            else if( aLocation && actual == closest_dist )
            {
                if( ( pn - aSeg.A ).SquaredEuclideanNorm()
                    < ( nearest - aSeg.A ).SquaredEuclideanNorm() )
                {
                    nearest = pn;
                }
            }
        }
    }

    if( closest_dist == 0 || closest_dist < aClearance )
    {
        if( aLocation )
            *aLocation = nearest;

        if( aActual )
            *aActual = closest_dist;

        return true;
    }

    return false;
}


void SHAPE_COMPOUND::TransformToPolygon( SHAPE_POLY_SET& aBuffer, int aError,
                                         ERROR_LOC aErrorLoc ) const
{
    for( SHAPE* item : m_shapes )
        item->TransformToPolygon( aBuffer, aError, aErrorLoc );
}
