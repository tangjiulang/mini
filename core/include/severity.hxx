#ifndef REPORT_SEVERITY_H
#define REPORT_SEVERITY_H

#include <string>

// Note: On windows, SEVERITY_ERROR collides with a system declaration,
// so we used RPT_SEVERITY_xxx instead of SEVERITY_xxx
enum SEVERITY
{
    RPT_SEVERITY_UNDEFINED = 0x01,
    RPT_SEVERITY_INFO = 0x02,
    RPT_SEVERITY_EXCLUSION = 0x04,
    RPT_SEVERITY_ACTION = 0x08,
    RPT_SEVERITY_WARNING = 0x10,
    RPT_SEVERITY_ERROR = 0x20,
    RPT_SEVERITY_IGNORE = 0x40,
    RPT_SEVERITY_DEBUG = 0x80,
};

/**
 * Convert a severity mask to a human-readable comma-separated string.
 * @param aSeverities A bitmask of SEVERITY values
 * @return A translated string like "Errors, Warnings"
 */
std::string formatSeverities(int aSeverities);

#endif // REPORT_SEVERITY_H
