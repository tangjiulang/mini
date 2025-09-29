#include "spdlog/spdlog.h"
#include <bezier_curves.hxx>
#include <ellipse.hxx>
#include <trigo.hxx>
#include <vector2d.hxx>  // for VECTOR2D, operator*, VECTOR2

#define BEZIER_DBG "bezier"

BEZIER_POLY::BEZIER_POLY( const VECTOR2I& aStart, const VECTOR2I& aCtrl1,
                          const VECTOR2I& aCtrl2, const VECTOR2I& aEnd )
{
    m_ctrlPts.emplace_back( VECTOR2D( aStart ) );
    m_ctrlPts.emplace_back( VECTOR2D( aCtrl1 ) );
    m_ctrlPts.emplace_back( VECTOR2D( aCtrl2 ) );
    m_ctrlPts.emplace_back( VECTOR2D( aEnd ) );

    m_minSegLen = 0.0;
}


BEZIER_POLY::BEZIER_POLY( const std::vector<VECTOR2I>& aControlPoints )
{
    m_ctrlPts.reserve( aControlPoints.size() );

    for( const VECTOR2I& pt : aControlPoints )
        m_ctrlPts.emplace_back( pt );

    m_minSegLen = 0.0;
}


bool BEZIER_POLY::isNaN() const
{
    for( const VECTOR2D& pt : m_ctrlPts )
    {
        if( std::isnan( pt.x ) || std::isnan( pt.y ) )
            return true;
    }

    return false;
}


bool BEZIER_POLY::isFlat( double aMaxError ) const
{
    if( m_ctrlPts.size() == 3 )
    {
        VECTOR2D D21 = m_ctrlPts[1] - m_ctrlPts[0];
        VECTOR2D D31 = m_ctrlPts[2] - m_ctrlPts[0];

        double   t = D21.Dot( D31 ) / D31.SquaredEuclideanNorm();
        double   u = std::min( std::max( t, 0.0 ), 1.0 );
        VECTOR2D p = m_ctrlPts[0] + u * D31;
        VECTOR2D delta = m_ctrlPts[1] - p;

        return 0.5 * delta.EuclideanNorm() <= aMaxError;
    }
    else if( m_ctrlPts.size() == 4 )
    {
        VECTOR2D delta = m_ctrlPts[3] - m_ctrlPts[0];

        VECTOR2D D21 = m_ctrlPts[1] - m_ctrlPts[0];
        VECTOR2D D31 = m_ctrlPts[2] - m_ctrlPts[0];

        double cross1 = delta.Cross( D21 );
        double cross2 = delta.Cross( D31 );

        double inv_delta_sq = 1.0 / delta.SquaredEuclideanNorm();
        double d1 = ( cross1 * cross1 ) * inv_delta_sq;
        double d2 = ( cross2 * cross2 ) * inv_delta_sq;

        double factor = ( cross1 * cross2 > 0.0 ) ? 3.0 / 4.0 : 4.0 / 9.0;
        double f2 = factor * factor;
        double tol = aMaxError * aMaxError;

        return d1 * f2 <= tol && d2 * f2 <= tol;
    }

    assert( false );
    return false;

}


void BEZIER_POLY::GetPoly( std::vector<VECTOR2I>& aOutput, int aMaxError )
{
    aOutput.clear();
    std::vector<VECTOR2D> buffer;
    GetPoly( buffer, aMaxError );

    aOutput.reserve( buffer.size() );

    for( const VECTOR2D& pt : buffer )
        aOutput.emplace_back( KiROUND( pt.x ), KiROUND( pt.y ) );
}

static double approx_int( double x )
{
    const double d = 0.6744897501960817;
    const double d4 = d * d * d * d;
    return x / ( 1.0 - d + std::pow( d4 + x * x * 0.25, 0.25 ) );
}

static double approx_inv_int( double x )
{
    const double p = 0.39538816;
    return x * ( 1.0 - p + std::sqrt( p * p + 0.25 * x * x ) );
}


VECTOR2D BEZIER_POLY::eval( double t )
{
    double omt = 1.0 - t;
    double omt2 = omt * omt;

    if( m_ctrlPts.size() == 3 )
    {
        return omt2 * m_ctrlPts[0] + 2.0 * omt * t * m_ctrlPts[1] + t * t * m_ctrlPts[2];
    }
    else if( m_ctrlPts.size() == 4 )
    {
        double omt3 = omt * omt2;
        double t2 = t * t;
        double t3 = t * t2;
        return omt3 * m_ctrlPts[0] + 3.0 * t * omt2 * m_ctrlPts[1]
               + 3.0 * t2 * omt * m_ctrlPts[2] + t3 * m_ctrlPts[3];
    }
    else
    {
        assert( false );
        return VECTOR2D( 0, 0 );
    }
}

void BEZIER_POLY::getQuadPoly( std::vector<VECTOR2D>& aOutput, double aMaxError )
{
    double ddx = 2 * m_ctrlPts[1].x - m_ctrlPts[0].x - m_ctrlPts[2].x;
    double ddy = 2 * m_ctrlPts[1].y - m_ctrlPts[0].y - m_ctrlPts[2].y;
    double u0 =
            ( m_ctrlPts[1].x - m_ctrlPts[0].x ) * ddx + ( m_ctrlPts[1].y - m_ctrlPts[0].y ) * ddy;
    double u2 =
            ( m_ctrlPts[2].x - m_ctrlPts[1].x ) * ddx + ( m_ctrlPts[2].y - m_ctrlPts[1].y ) * ddy;
    double cross =
            ( m_ctrlPts[2].x - m_ctrlPts[0].x ) * ddy - ( m_ctrlPts[2].y - m_ctrlPts[0].y ) * ddx;
    double x0 = u0 / cross;
    double x2 = u2 / cross;
    double scale = std::abs( cross ) / ( std::hypot( ddx, ddy ) * std::abs( x2 - x0 ) );

    double a0 = approx_int( x0 );
    double a2 = approx_int( x2 );

    int n = std::ceil( 0.5 * std::abs( a2 - a0 ) * std::sqrt( scale / aMaxError ) );

    double v0 = approx_inv_int( a0 );
    double v2 = approx_inv_int( a2 );

    aOutput.emplace_back( m_ctrlPts[0] );

    for( int ii = 0; ii < n; ++ii )
    {
        double u = approx_inv_int( a0 + ( a2 - a0 ) * ii / n );
        double t = ( u - v0 ) / ( v2 - v0 );
        aOutput.emplace_back( eval( t ) );
    }

    aOutput.emplace_back( m_ctrlPts[2] );
}


int BEZIER_POLY::numberOfInflectionPoints()
{
    VECTOR2D D21 = m_ctrlPts[1] - m_ctrlPts[0];
    VECTOR2D D32 = m_ctrlPts[2] - m_ctrlPts[1];
    VECTOR2D D43 = m_ctrlPts[3] - m_ctrlPts[2];

    double cross1 = D21.Cross( D32 ) * D32.Cross( D43 );
    double cross2 = D21.Cross( D32 ) * D21.Cross( D43 );

    if( cross1 < 0.0 )
        return 1;
    else if( cross2 > 0.0 )
        return 0;

    bool b1 = D21.Dot( D32 ) > 0.0;
    bool b2 = D32.Dot( D43 ) > 0.0;

    if( b1 ^ b2 )
        return 0;

    spdlog::trace("{} numberOfInflectionPoints: rare case", BEZIER_DBG);
    // These are rare cases where there are potentially 2 or 0 inflection points.
    return -1;
}


double BEZIER_POLY::thirdControlPointDeviation()
{
    VECTOR2D delta = m_ctrlPts[1] - m_ctrlPts[0];
    double len_sq = delta.SquaredEuclideanNorm();

    if( len_sq < 1e-6 )
        return 0.0;

    double len = std::sqrt( len_sq );
    double r = ( m_ctrlPts[1].y - m_ctrlPts[0].y ) / len;
    double s = ( m_ctrlPts[0].x - m_ctrlPts[1].x ) / len;
    double u = ( m_ctrlPts[1].x * m_ctrlPts[0].y - m_ctrlPts[0].x * m_ctrlPts[1].y ) / len;

    return std::abs( r * m_ctrlPts[2].x + s * m_ctrlPts[2].y + u );
}


void BEZIER_POLY::subdivide( double aT, BEZIER_POLY& aLeft, BEZIER_POLY& aRight )
{
    if( m_ctrlPts.size() == 3 )
    {
        aLeft.m_ctrlPts[0] = m_ctrlPts[0];
        aLeft.m_ctrlPts[1] = m_ctrlPts[0] + aT * ( m_ctrlPts[1] - m_ctrlPts[0] );
        aLeft.m_ctrlPts[2] = eval( aT );

        aRight.m_ctrlPts[2] = m_ctrlPts[2];
        aRight.m_ctrlPts[1] = m_ctrlPts[1] + aT * ( m_ctrlPts[2] - m_ctrlPts[1] );
        aRight.m_ctrlPts[0] = aLeft.m_ctrlPts[2];
    }
    else if( m_ctrlPts.size() == 4 )
    {
        VECTOR2D left_ctrl1 = m_ctrlPts[0] + aT * ( m_ctrlPts[1] - m_ctrlPts[0] );
        VECTOR2D tmp = m_ctrlPts[1] + aT * ( m_ctrlPts[2] - m_ctrlPts[1] );
        VECTOR2D left_ctrl2 = left_ctrl1 + aT * ( tmp - left_ctrl1 );
        VECTOR2D right_ctrl2 = m_ctrlPts[2] + aT * ( m_ctrlPts[3] - m_ctrlPts[2] );
        VECTOR2D right_ctrl1 = tmp + aT * ( right_ctrl2 - tmp );
        VECTOR2D shared = left_ctrl2 + aT * ( right_ctrl1 - left_ctrl2 );

        aLeft.m_ctrlPts[0] = m_ctrlPts[0];
        aLeft.m_ctrlPts[1] = left_ctrl1;
        aLeft.m_ctrlPts[2] = left_ctrl2;
        aLeft.m_ctrlPts[3] = shared;

        aRight.m_ctrlPts[3] = m_ctrlPts[3];
        aRight.m_ctrlPts[2] = right_ctrl2;
        aRight.m_ctrlPts[1] = right_ctrl1;
        aRight.m_ctrlPts[0] = shared;
    }
    else
    {
        assert( false );
    }
}


void BEZIER_POLY::recursiveSegmentation( std::vector<VECTOR2D>& aOutput, double aThreshhold )
{
    spdlog::trace("{} recursiveSegmentation with threshold {}", BEZIER_DBG, aThreshhold );
    std::vector<BEZIER_POLY> stack;

    BEZIER_POLY* bezier = nullptr;
    BEZIER_POLY left( std::vector<VECTOR2D>(4) );
    BEZIER_POLY right( std::vector<VECTOR2D>(4) );

    stack.push_back( *this );

    while( !stack.empty() )
    {
        bezier = &stack.back();

        if( bezier->m_ctrlPts[3] == bezier->m_ctrlPts[0] )
        {
            spdlog::trace("{} recursiveSegmentation dropping zero length segment", BEZIER_DBG);
            stack.pop_back();
        }
        else if( bezier->isFlat( aThreshhold ) )
        {
            aOutput.push_back( bezier->m_ctrlPts[3] );
            stack.pop_back();
        }
        else
        {
            bezier->subdivide( 0.5, left, right );
            *bezier = right;
            stack.push_back( left );
        }
    }

    spdlog::trace("{} recursiveSegmentation generated {} points", BEZIER_DBG, aOutput.size() );
}


int BEZIER_POLY::findInflectionPoints( double& aT1, double& aT2 )
{
    VECTOR2D A{ ( -m_ctrlPts[0].x + 3 * m_ctrlPts[1].x - 3 * m_ctrlPts[2].x + m_ctrlPts[3].x ),
                ( -m_ctrlPts[0].y + 3 * m_ctrlPts[1].y - 3 * m_ctrlPts[2].y + m_ctrlPts[3].y ) };
    VECTOR2D B{ ( 3 * m_ctrlPts[0].x - 6 * m_ctrlPts[1].x + 3 * m_ctrlPts[2].x ),
                ( 3 * m_ctrlPts[0].y - 6 * m_ctrlPts[1].y + 3 * m_ctrlPts[2].y ) };
    VECTOR2D C{ ( -3 * m_ctrlPts[0].x + 3 * m_ctrlPts[1].x ),
                ( -3 * m_ctrlPts[0].y + 3 * m_ctrlPts[1].y ) };

    double a = 3 * A.Cross( B );
    double b = 3 * A.Cross( C );
    double c = B.Cross( C );

    // Solve the quadratic equation a*t^2 + b*t + c = 0
    double r2 = ( b * b - 4 * a * c );

    aT1 = 0.0;
    aT2 = 0.0;

    if( r2 >= 0.0 && a != 0.0 )
    {
        double r = std::sqrt( r2 );

        double t1 = ( ( -b + r ) / ( 2 * a ) );
        double t2 = ( ( -b - r ) / ( 2 * a ) );

        if( ( t1 > 0.0 && t1 < 1.0 ) && ( t2 > 0.0 && t2 < 1.0 ) )
        {
            if( t1 > t2 )
            {
                std::swap( t1, t2 );
            }

            aT1 = t1;
            aT2 = t2;

            if( t2 - t1 > 0.00001 )
            {
                spdlog::trace("{} BEZIER_POLY Found 2 inflection points at t1 = {}, t2 = {}", BEZIER_DBG, t1, t2 );
                return 2;
            }
            else
            {
                spdlog::trace("{} BEZIER_POLY Found 1 inflection point at t = {}", BEZIER_DBG, t1 );
                return 1;
            }
        }
        else if( t1 > 0.0 && t1 < 1.0 )
        {
            aT1 = t1;
            spdlog::trace("{} BEZIER_POLY Found 1 inflection point at t = {}", BEZIER_DBG, t1 );
            return 1;
        }
        else if( t2 > 0.0 && t2 < 1.0 )
        {
            aT1 = t2;
            spdlog::trace("{} BEZIER_POLY Found 1 inflection point at t = {}", BEZIER_DBG, t2);
            return 1;
        }

        spdlog::trace("{} BEZIER_POLY Found no inflection points", BEZIER_DBG);
        return 0;
    }

    spdlog::trace("{} BEZIER_POLY Found no inflection points", BEZIER_DBG);
    return 0;
}


void BEZIER_POLY::cubicParabolicApprox( std::vector<VECTOR2D>& aOutput, double aMaxError )
{
    std::vector<BEZIER_POLY> stack;
    stack.push_back( std::vector<VECTOR2D>(4) );
    stack.push_back( std::vector<VECTOR2D>(4) );
    stack.push_back( std::vector<VECTOR2D>(4) );
    stack.push_back( std::vector<VECTOR2D>(4) );

    BEZIER_POLY* c = this;
    BEZIER_POLY* b1 = &stack[0];
    BEZIER_POLY* b2 = &stack[1];

    for( ;; )
    {
        if( c->isNaN() )
        {
            spdlog::trace( "cubicParabolicApprox: NaN detected" );
            break;
        }

        if( c->isFlat( aMaxError ) )
        {
            spdlog::trace("{} cubicParabolicApprox: General Flatness detected, adding {} {}", BEZIER_DBG, c->m_ctrlPts[3].x, c->m_ctrlPts[3].y );
            // If the subsegment deviation satisfies the flatness criterion, store the last point and stop
            aOutput.push_back( c->m_ctrlPts[3] );
            break;
        }

        // Find the third control point deviation and the t values for subdivision
        double d = c->thirdControlPointDeviation();
        double t = 2 * std::sqrt( aMaxError / ( 3.0 * d ) ); // Forumla 2 in Hain et al.

        spdlog::trace("{} cubicParabolicApprox: split point t = {}", BEZIER_DBG, t );

        if( t > 1.0 )
        {
            spdlog::trace("{} cubicParabolicApprox: Invalid t value detected", BEZIER_DBG);
            // Case where the t value calculated is invalid, so use recursive subdivision
            c->recursiveSegmentation( aOutput, aMaxError );
            break;
        }

        // Valid t value to subdivide at that calculated value
        c->subdivide( t, *b1, *b2 );

        // First subsegment should have its deviation equal to flatness
        if( b1->isFlat( aMaxError ) )
        {
            spdlog::trace("{} cubicParabolicApprox: Flatness detected, adding {} {}", BEZIER_DBG, b1->m_ctrlPts[3].x, b1->m_ctrlPts[3].y );
            aOutput.push_back( b1->m_ctrlPts[3] );
        }
        else
        {
            // if not then use segment to handle any mathematical errors
            b1->recursiveSegmentation( aOutput, aMaxError );
        }

        // Repeat the process for the left over subsegment
        c = b2;

        if( b1 == &stack.front() )
        {
            b1 = &stack[2];
            b2 = &stack[3];
        }
        else
        {
            b1 = &stack[0];
            b2 = &stack[1];
        }
    }
}


void BEZIER_POLY::getCubicPoly( std::vector<VECTOR2D>& aOutput, double aMaxError )
{
    aOutput.push_back( m_ctrlPts[0] );

    if( numberOfInflectionPoints() == 0 )
    {
        spdlog::trace("{} getCubicPoly Short circuit to PA", BEZIER_DBG);
        // If no inflection points then apply PA on the full Bezier segment.
        cubicParabolicApprox( aOutput, aMaxError );
        return;
    }

    // If one or more inflection points then we will have to subdivide the curve
    double t1, t2;
    int    numOfIfP = findInflectionPoints( t1, t2 );

    if( numOfIfP == 2 )
    {
        spdlog::trace("{} getCubicPoly: 2 inflection points", BEZIER_DBG);
        // Case when 2 inflection points then divide at the smallest one first
        BEZIER_POLY sub1( std::vector<VECTOR2D>( 4 ) );
        BEZIER_POLY tmp1( std::vector<VECTOR2D>( 4 ) );
        BEZIER_POLY sub2( std::vector<VECTOR2D>( 4 ) );
        BEZIER_POLY sub3( std::vector<VECTOR2D>( 4 ) );

        subdivide( t1, sub1, tmp1 );

        // Now find the second inflection point in the second curve and subdivide
        numOfIfP = tmp1.findInflectionPoints( t1, t2 );
        if( numOfIfP == 2 )
            tmp1.subdivide( t1, sub2, sub3 );
        else if( numOfIfP == 1 )
            tmp1.subdivide( t1, sub2, sub3 );
        else
        {
            spdlog::trace( "{} getCubicPoly: 2nd inflection point not found", BEZIER_DBG);
            return;
        }

        // Use PA for first subsegment
        sub1.cubicParabolicApprox( aOutput, aMaxError );

        // Use Segment for the second (middle) subsegment
        sub2.recursiveSegmentation( aOutput, aMaxError );

        // Use PA for the third curve
        sub3.cubicParabolicApprox( aOutput, aMaxError );
    }
    else if( numOfIfP == 1 )
    {
        spdlog::trace("{} getCubicPoly: 1 inflection point", BEZIER_DBG);
        // Case where there is one inflection point, subdivide once and use PA on both subsegments
        BEZIER_POLY sub1( std::vector<VECTOR2D>( 4 ) );
        BEZIER_POLY sub2( std::vector<VECTOR2D>( 4 ) );
        subdivide( t1, sub1, sub2 );
        sub1.cubicParabolicApprox( aOutput, aMaxError );
        sub2.cubicParabolicApprox( aOutput, aMaxError );
    }
    else
    {
        spdlog::trace("{} getCubicPoly: Unknown inflection points", BEZIER_DBG);
        // Case where there is no inflection, use PA directly
        cubicParabolicApprox( aOutput, aMaxError );
    }
}


void BEZIER_POLY::GetPoly( std::vector<VECTOR2D>& aOutput, double aMaxError )
{
    if( aMaxError <= 0.0 )
        aMaxError = 10.0;

    if( m_ctrlPts.size() == 3 )
    {
        getQuadPoly( aOutput, aMaxError );
    }
    else if( m_ctrlPts.size() == 4 )
    {
        getCubicPoly( aOutput, aMaxError );
    }
    else
    {
        assert( false );
    }

    spdlog::trace("{} GetPoly generated {} points", BEZIER_DBG, aOutput.size() );
}


template<typename T>
void TransformEllipseToBeziers( const ELLIPSE<T>& aEllipse, std::vector<BEZIER<T>>& aBeziers )
{
    EDA_ANGLE arcAngle = -( aEllipse.EndAngle - aEllipse.StartAngle );

    if( arcAngle >= ANGLE_0 )
        arcAngle -= ANGLE_360;

    /*
     * KiCad does not natively support ellipses or elliptical arcs.  So, we convert them to Bezier
     * splines as these are the nearest thing we have that represents them in a way that is both
     * editable and preserves their curvature accurately (enough).
     *
     * Credit to Kliment for developing and documenting this method.
     */
    /// Minimum number of Beziers to use for a full circle to keep error manageable.
    const int minBeziersPerCircle = 4;

    /// The number of Beziers needed for the given arc
    const int numBeziers = static_cast<int>(
            std::ceil( std::abs( arcAngle.AsRadians() / ( 2 * M_PI / minBeziersPerCircle ) ) ) );

    /// Angle occupied by each Bezier
    const double angleIncrement = arcAngle.AsRadians() / numBeziers;

    /*
     * Now, let's assume a circle of radius 1, centered on origin, with angle startangle
     * x-axis-aligned. We'll move, scale, and rotate it later. We're creating Bezier curves that hug
     * this circle as closely as possible, with the angles that will be used on the final ellipse
     * too.
     *
     * Thanks to the beautiful and excellent https://pomax.github.io/bezierinfo we know how to
     * define a curve that hugs a circle as closely as possible.
     *
     * We need the value k, which is the optimal distance from the endpoint to the control point to
     * make the curve match the circle for a given circle arc angle.
     *
     * k = 4/3 * tan(θ/4), where θ is the angle of the arc. In our case, θ=angleIncrement
     */
    double theta = angleIncrement;
    double k     = ( 4. / 3. ) * std::tan( theta / 4 );

    /*
     * Define our Bezier:
     * - Start point is on the circle at the x-axis
     * - First control point just uses k as the y-value
     * - Second control point is offset from the end point
     * - End point is defined by the angle of the arc segment
     * Note that we use double here no matter what the template param is; round at the end only.
     */
    BEZIER<double> first = { { 1, 0 },
                             { 1, k },
                             { std::cos( theta ) + k * std::sin( theta ),
                               std::sin( theta ) - k * std::cos( theta ) },
                             { std::cos( theta ), std::sin( theta ) } };

    /*
     * Now construct the actual segments by transforming/rotating the first one
     */
    auto transformPoint =
            [&]( VECTOR2D aPoint, const double aAngle ) -> VECTOR2D
            {
                // Bring to the actual starting angle
                RotatePoint( aPoint,
                             -EDA_ANGLE( aAngle - aEllipse.StartAngle.AsRadians(), RADIANS_T ) );

                // Then scale to the major and minor radiuses of the ellipse
                aPoint *= VECTOR2D( aEllipse.MajorRadius, aEllipse.MinorRadius );

                // Now rotate to the ellipse coordinate system
                RotatePoint( aPoint, -aEllipse.Rotation );

                // And finally offset to the center location of the ellipse
                aPoint += aEllipse.Center;

                return aPoint;
            };

    for( int i = 0; i < numBeziers; i++ )
    {
        aBeziers.emplace_back( BEZIER<T>( {
                transformPoint( first.Start, i * angleIncrement ),
                transformPoint( first.C1,    i * angleIncrement ),
                transformPoint( first.C2,    i * angleIncrement ),
                transformPoint( first.End,   i * angleIncrement )
        } ) );
    }
}


template void TransformEllipseToBeziers( const ELLIPSE<double>& aEllipse,
                                         std::vector<BEZIER<double>>& aBeziers );
template void TransformEllipseToBeziers( const ELLIPSE<int>& aEllipse,
                                         std::vector<BEZIER<int>>& aBeziers );