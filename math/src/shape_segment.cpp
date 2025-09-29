#include <sstream>

#include <shape_segment.hxx>
#include <shape_circle.hxx>
#include <convert_basic_shapes_to_polygon.hxx>

const std::string SHAPE_SEGMENT::Format( bool aCplusPlus ) const
{
    std::stringstream ss;

    if( aCplusPlus )
    {
    ss << "SHAPE_SEGMENT( VECTOR2I( ";
    ss << m_seg.A.x;
    ss << ", ";
    ss << m_seg.A.y;
    ss << "), VECTOR2I( ";
    ss << m_seg.B.x;
    ss << ", ";
    ss << m_seg.B.y;
    ss << "), ";
    ss << m_width;
    ss << "); ";
    }
    else
    {
        ss << SHAPE::Format( aCplusPlus ) << " ";
        ss << m_seg.A.x;
        ss << " ";
        ss << m_seg.A.y;
        ss << " ";
        ss << m_seg.B.x;
        ss << " ";
        ss << m_seg.B.y;
        ss << " ";
        ss << m_width;
    }

    return ss.str();
}

const std::string SHAPE_CIRCLE::Format( bool aCplusPlus ) const
{
    std::stringstream ss;

    if( aCplusPlus )
    {
        ss << "SHAPE_CIRCLE( VECTOR2I( ";
        ss << m_circle.Center.x;
        ss << ", ";
        ss << m_circle.Center.y;
        ss << "), ";
        ss << m_circle.Radius;
        ss << "); ";
    }   else
    {
        ss << SHAPE::Format( aCplusPlus ) << " ";
        ss << m_circle.Center.x;
        ss << " ";
        ss << m_circle.Center.y;
        ss << " ";
        ss << m_circle.Radius;
    }
    return ss.str();
}


void SHAPE_CIRCLE::TransformToPolygon( SHAPE_POLY_SET& aBuffer, int aError,
                                       ERROR_LOC aErrorLoc ) const
{
    TransformCircleToPolygon( aBuffer, m_circle.Center, m_circle.Radius, aError, aErrorLoc );
}


bool SHAPE_SEGMENT::Is45Degree( EDA_ANGLE aTollerance ) const
{
    EDA_ANGLE mag = EDA_ANGLE( m_seg.A - m_seg.B ).Normalize180();

    double f = fmod( mag.AsDegrees(), 45.0 );
    double d = aTollerance.AsDegrees();

    if( f >= 45.0 - d || f <= d )
    {
        return true;
    }

    return false;
}


void SHAPE_SEGMENT::TransformToPolygon( SHAPE_POLY_SET& aBuffer, int aError,
                                        ERROR_LOC aErrorLoc ) const
{
    TransformOvalToPolygon( aBuffer, m_seg.A, m_seg.B, m_width, aError, aErrorLoc );
}
