#include <eda_angle.hxx>


EDA_ANGLE EDA_ANGLE::KeepUpright() const
{
    EDA_ANGLE inAngle( *this );
    inAngle.Normalize();

    double inDegrees = inAngle.AsDegrees();
    double outDegrees;

    if( inDegrees <= 45 || inDegrees >= 315 || ( inDegrees > 135 && inDegrees <= 225 ) )
        outDegrees = 0;
    else
        outDegrees = 90;

    return EDA_ANGLE( outDegrees, DEGREES_T );
}


bool EDA_ANGLE::IsCardinal() const
{
    double test = m_value;

    while( test < 0.0 )
        test += 90.0;

    while( test >= 90.0 )
        test -= 90.0;

    return test == 0.0;
}


bool EDA_ANGLE::IsCardinal90() const
{
    // return true if angle is one of the two cardinal directions (90/270 degrees),
    double test = std::abs( m_value );

    while( test >= 180.0 )
        test -= 180.0;

    return test == 90.0;
}
