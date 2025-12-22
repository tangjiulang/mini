#include <dpi_scaling.hxx>


double DPI_SCALING::GetMaxScaleFactor()
{
    // displays with higher than 4.0 DPI are not really going to be useful
    // for KiCad (even an 8k display would be effectively only ~1080p at 4x)
    return 6.0;
}


double DPI_SCALING::GetMinScaleFactor()
{
    // scales under 1.0 don't make sense from a HiDPI perspective
    return 1.0;
}


double DPI_SCALING::GetDefaultScaleFactor()
{
    // no scaling => 1.0
    return 1.0;
}
