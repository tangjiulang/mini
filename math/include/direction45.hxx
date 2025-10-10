#ifndef DIRECTION45_H
#define DIRECTION45_H

#include <seg.hxx>
#include <shape_line_chain.hxx>
#include <vector2d.hxx>

// believe or not, X11 headers have a F****ING macro called Opposite...
#undef Opposite

/**
 * Represent route directions & corner angles in a 45-degree metric.
 */
class DIRECTION_45
{
public:

    /**
     * Available directions, there are 8 of them, as on a rectilinear map (north = up) +
     * an extra undefined direction, reserved for traces that don't respect 45-degree routing
     * regime.
     *
     * @note North represents "up" to the user looking at the application, which is the negative-y
     * direction in the world coordinate space!
     */
    enum Directions : int
    {
        N           = 0,
        NE          = 1,
        E           = 2,
        SE          = 3,
        S           = 4,
        SW          = 5,
        W           = 6,
        NW          = 7,
        LAST        = 8,
        UNDEFINED   = -1
    };

    /**
     * Corner modes.
     * A corner can either be 45° or 90° and can be fillet/rounded or mitered
     */
    enum CORNER_MODE
    {
        MITERED_45 = 0, ///< H/V/45 with mitered corners (default)
        ROUNDED_45 = 1, ///< H/V/45 with filleted corners
        MITERED_90 = 2, ///< H/V only (90-degree corners)
        ROUNDED_90 = 3, ///< H/V with filleted corners
    };

    /**
     * Represent kind of angle formed by vectors heading in two DIRECTION_45s.
     */
    enum AngleType
    {
        ANG_OBTUSE      = 0x01,
        ANG_RIGHT       = 0x02,
        ANG_ACUTE       = 0x04,
        ANG_STRAIGHT    = 0x08,
        ANG_HALF_FULL   = 0x10,
        ANG_UNDEFINED   = 0x20
    };

    DIRECTION_45( Directions aDir = UNDEFINED ) : m_dir( aDir ), m_90deg( false ) {}

    /**
     * @param aVec vector in world space, whose direction will be translated into a DIRECTION_45.
     */
    DIRECTION_45( const VECTOR2I &aVec, bool a90 = false ) :
            m_90deg( a90 )
    {
        VECTOR2I vec( aVec );
        vec.y = -vec.y;
        construct_( vec );
    }

    /**
     * @param aSeg segment, whose direction will be translated into a DIRECTION_45.
     */
    DIRECTION_45( const SEG& aSeg, bool a90 = false ) :
            m_90deg( a90 )
    {
        VECTOR2I vec( aSeg.B - aSeg.A );
        vec.y = -vec.y;
        construct_( vec );
    }

    /**
     * Create a DIRECTION_45 from the endpoints of a given arc.
     *
     * @param aArc will be translated into the closest DIRECTION_45
     */
    DIRECTION_45( const SHAPE_ARC& aArc, bool a90 = false ) :
            m_90deg( a90 )
    {
        VECTOR2I vec( aArc.GetP1() - aArc.GetP0() );
        vec.y = -vec.y;
        construct_( vec );
    }

    /**
     * Format the direction in a human readable word.
     *
     * @return name of the direction
     */
    const std::string Format() const
    {
        switch( m_dir )
        {
        case N:
            return "north";

        case NE:
            return "north-east";

        case E:
            return "east";

        case SE:
            return "south-east";

        case S:
            return "south";

        case SW:
            return "south-west";

        case W:
            return "west";

        case NW:
            return "north-west";

        case UNDEFINED:
            return "undefined";

        default:
            return "<Error>";
        }
    }

    /**
     * Return a direction opposite (180 degree) to (this).
     *
     * @return opposite direction
     */
    DIRECTION_45 Opposite() const
    {
        const Directions OppositeMap[] = { S, SW, W, NW, N, NE, E, SE, UNDEFINED };
        return OppositeMap[m_dir];
    }

    /**
     * Return the type of angle between directions (this) and \a aOther.
     *
     * @param aOther direction to compare angle with
     */
    AngleType Angle( const DIRECTION_45& aOther ) const
    {
        if( m_dir == UNDEFINED || aOther.m_dir == UNDEFINED )
            return ANG_UNDEFINED;

        int d = std::abs( m_dir - aOther.m_dir );

        if( d == 1 || d == 7 )
            return ANG_OBTUSE;
        else if( d == 2 || d == 6 )
            return ANG_RIGHT;
        else if( d == 3 || d == 5 )
            return ANG_ACUTE;
        else if( d == 4 )
            return ANG_HALF_FULL;
        else
            return ANG_STRAIGHT;
    }

    /**
     * @return true, when (this) forms an obtuse angle with \a aOther.
     */
    bool IsObtuse( const DIRECTION_45& aOther ) const
    {
        return Angle( aOther ) == ANG_OBTUSE;
    }

    /**
     * Returns true if the direction is diagonal (e.g. North-West, South-East, etc).
     *
     * @return true, when diagonal.
     */
    bool IsDiagonal() const
    {
        return ( m_dir % 2 ) == 1;
    }

    bool IsDefined() const
    {
        return m_dir != UNDEFINED;
    }

    /**
     * Build a 2-segment line chain between points aP0 and aP1 and following 45-degree routing
     * regime. If aStartDiagonal is true, the trace starts with a diagonal segment.
     *
     * @param aP0 starting point
     * @param aP1 ending point
     * @param aStartDiagonal whether the first segment has to be diagonal
     * @param aMode How the corner is made. If it is a 90° corner, aStartDiagonal means
     *  start with the shorter direction first / use arc before the straight segment.
     * @return the trace
     */
    const SHAPE_LINE_CHAIN BuildInitialTrace( const VECTOR2I& aP0, const VECTOR2I& aP1,
                                              bool        aStartDiagonal = false,
                                              CORNER_MODE aMode = CORNER_MODE::MITERED_45 ) const;

    bool operator==( const DIRECTION_45& aOther ) const
    {
        return aOther.m_dir == m_dir;
    }

    bool operator!=( const DIRECTION_45& aOther ) const
    {
        return aOther.m_dir != m_dir;
    }

    /**
     * Return the direction on the right side of this (i.e. turns right by 45 or 90 deg).
     */
    const DIRECTION_45 Right() const
    {
        DIRECTION_45 r;

        if ( m_dir != UNDEFINED )
        {
            if( m_90deg )
                r.m_dir = static_cast<Directions>( ( m_dir + 2 ) % LAST );
            else
                r.m_dir = static_cast<Directions>( ( m_dir + 1 ) % LAST );
        }

        return r;
    }

    /**
     * Return the direction on the left side of this (i.e. turns left by 45 or 90 deg).
     */
    const DIRECTION_45 Left() const
    {
        DIRECTION_45 l;

        if ( m_dir != UNDEFINED )
        {
            if( m_90deg )
                l.m_dir = static_cast<Directions>( ( m_dir + LAST - 2 ) % LAST );
            else
                l.m_dir = static_cast<Directions>( ( m_dir + LAST - 1 ) % LAST );
        }

        return l;
    }

    /**
     * @return a unit vector in world coordinate system corresponding to our direction.
     */
    const VECTOR2I ToVector() const
    {
        switch( m_dir )
        {
            case N: return VECTOR2I( 0, -1 );
            case S: return VECTOR2I( 0, 1 );
            case E: return VECTOR2I( 1, 0 );
            case W: return VECTOR2I( -1, 0 );
            case NE: return VECTOR2I( 1, -1 );
            case NW: return VECTOR2I( -1, -1 );
            case SE: return VECTOR2I( 1, 1 );
            case SW: return VECTOR2I( -1, 1 );

            default:
                return VECTOR2I( 0, 0 );
        }
    }

    int Mask() const
    {
        return 1 << ( (int) m_dir );
    }

private:
    /**
     * Calculate the direction from a vector. If the vector's angle is not a multiple of 45
     * degrees, the direction is rounded to the nearest octant.
     *
     * @param aVec our vector
     */
    void construct_( const VECTOR2I& aVec )
    {
        m_dir = UNDEFINED;

        if( aVec.x == 0 && aVec.y == 0 )
            return;

        double mag = 360.0 - ( 180.0 / M_PI * atan2( (double) aVec.y, (double) aVec.x ) ) + 90.0;

        if( mag >= 360.0 )
            mag -= 360.0;

        if( mag < 0.0 )
            mag += 360.0;

        int dir = ( mag + 22.5 ) / 45.0;

        if( dir >= LAST )
            dir -= LAST;

        if( dir < 0 )
            dir += LAST;

        m_dir = (Directions) dir;

        return;
    }

    ///< our actual direction
    Directions m_dir;

    ///< Are we routing on 45 or 90 degree increments
    bool m_90deg;
};

#endif    // DIRECTION45_H
