#include <outline_decomposer.hxx>
#include <bezier_curves.hxx>

using namespace KIFONT;

OUTLINE_DECOMPOSER::OUTLINE_DECOMPOSER( FT_Outline& aOutline ) :
        m_outline( aOutline ),
        m_contours( nullptr )
{
}


static VECTOR2D toVector2D( const FT_Vector* aFreeTypeVector )
{
    return VECTOR2D( (double) aFreeTypeVector->x * GLYPH_SIZE_SCALER,
                     (double) aFreeTypeVector->y * GLYPH_SIZE_SCALER );
}


void OUTLINE_DECOMPOSER::newContour()
{
    CONTOUR contour;
    contour.m_Orientation = FT_Outline_Get_Orientation( &m_outline );
    m_contours->push_back( std::move( contour ) );
}


void OUTLINE_DECOMPOSER::addContourPoint( const VECTOR2D& p )
{
    // don't add repeated points
    if( m_contours->back().m_Points.empty() || m_contours->back().m_Points.back() != p )
        m_contours->back().m_Points.push_back( p );
}


int OUTLINE_DECOMPOSER::moveTo( const FT_Vector* aEndPoint, void* aCallbackData )
{
    OUTLINE_DECOMPOSER* decomposer = static_cast<OUTLINE_DECOMPOSER*>( aCallbackData );

    decomposer->m_lastEndPoint = toVector2D( aEndPoint );

    decomposer->newContour();
    decomposer->addContourPoint( decomposer->m_lastEndPoint );

    return 0;
}


int OUTLINE_DECOMPOSER::lineTo( const FT_Vector* aEndPoint, void* aCallbackData )
{
    OUTLINE_DECOMPOSER* decomposer = static_cast<OUTLINE_DECOMPOSER*>( aCallbackData );

    decomposer->m_lastEndPoint = toVector2D( aEndPoint );

    decomposer->addContourPoint( decomposer->m_lastEndPoint );

    return 0;
}


int OUTLINE_DECOMPOSER::quadraticTo( const FT_Vector* aControlPoint, const FT_Vector* aEndPoint,
                                     void* aCallbackData )
{
    return cubicTo( aControlPoint, nullptr, aEndPoint, aCallbackData );
}


int OUTLINE_DECOMPOSER::cubicTo( const FT_Vector* aFirstControlPoint,
                                 const FT_Vector* aSecondControlPoint, const FT_Vector* aEndPoint,
                                 void* aCallbackData )
{
    OUTLINE_DECOMPOSER* decomposer = static_cast<OUTLINE_DECOMPOSER*>( aCallbackData );

    std::vector<VECTOR2D> bezier;
    bezier.push_back( decomposer->m_lastEndPoint );
    bezier.push_back( toVector2D( aFirstControlPoint ) );

    if( aSecondControlPoint )
    {
        // aSecondControlPoint == nullptr for quadratic Beziers
        bezier.push_back( toVector2D( aSecondControlPoint ) );
    }

    bezier.push_back( toVector2D( aEndPoint ) );

    std::vector<VECTOR2D> result;
    BEZIER_POLY           converter( bezier );
    converter.GetPoly( result, 2 );

    for( const VECTOR2D& p : result )
        decomposer->addContourPoint( p );

    decomposer->m_lastEndPoint = toVector2D( aEndPoint );

    return 0;
}


bool OUTLINE_DECOMPOSER::OutlineToSegments( std::vector<CONTOUR>* aContours )
{
    m_contours = aContours;

    FT_Outline_Funcs callbacks;

    callbacks.move_to = moveTo;
    callbacks.line_to = lineTo;
    callbacks.conic_to = quadraticTo;
    callbacks.cubic_to = cubicTo;
    callbacks.shift = 0;
    callbacks.delta = 0;

    FT_Error e = FT_Outline_Decompose( &m_outline, &callbacks, this );

    if( e )
        return false;

    for( CONTOUR& c : *m_contours )
        c.m_Winding = winding( c.m_Points );

    return true;
}


int OUTLINE_DECOMPOSER::winding( const std::vector<VECTOR2D>& aContour ) const
{
    // -1 == counterclockwise, 1 == clockwise

    const int cw = 1;
    const int ccw = -1;

    if( aContour.size() < 2 )
    {
        // zero or one points, so not a clockwise contour - in fact not a contour at all
        //
        // It could also be argued that a contour needs 3 extremum points at a minimum to be
        // considered a proper contour (ie. a glyph (subpart) outline, or a hole)
        return 0;
    }

    double sum = 0.0;
    size_t len = aContour.size();

    for( size_t i = 0; i < len - 1; i++ )
    {
        VECTOR2D p1 = aContour[ i ];
        VECTOR2D p2 = aContour[ i + 1 ];

        sum += ( ( p2.x - p1.x ) * ( p2.y + p1.y ) );
    }

    sum += ( ( aContour[0].x - aContour[len - 1].x ) * ( aContour[0].y + aContour[len - 1].y ) );

    if( sum > 0.0 )
        return cw;
    if( sum < 0.0 )
        return ccw;

    return 0;
}
