#ifndef VERSION_INFO_H_
#define VERSION_INFO_H_

#include <mutex>
#include <string>

namespace KIFONT
{
/**
 * Container for library version helpers.
 */
class VERSION_INFO
{
public:
    static std::string FontConfig();

    static std::string FreeType();

    static std::string HarfBuzz();

    static std::string FontLibrary();

private:
    // we are a static helper
    VERSION_INFO() {}
};

} //namespace KIFONT

#endif // VERSION_INFO_H_
