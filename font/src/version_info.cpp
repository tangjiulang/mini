#include <version_info.hxx>
#include <fontconfig.hxx>
#include <harfbuzz/hb.h>
#ifdef _MSC_VER
#include <ft2build.h>
#else
#include <freetype2/ft2build.h>
#endif
#include FT_FREETYPE_H

#include <format>

using namespace KIFONT;

std::string VERSION_INFO::FreeType()
{
    FT_Library library;

    FT_Int major = 0;
    FT_Int minor = 0;
    FT_Int patch = 0;
    FT_Init_FreeType( &library );
    FT_Library_Version( library, &major, &minor, &patch );
    FT_Done_FreeType( library );

    return std::format( "%d.%d.%d", major, minor, patch );
}


std::string VERSION_INFO::HarfBuzz()
{
    return std::format(HB_VERSION_STRING);
}


std::string VERSION_INFO::FontConfig()
{
    return fontconfig::FONTCONFIG::Version();
}


std::string VERSION_INFO::FontLibrary()
{
    return std::format("FreeType %s HarfBuzz %s", FreeType(), HarfBuzz());
}