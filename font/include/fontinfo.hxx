#ifndef FONT_FONTINFO_H
#define FONT_FONTINFO_H

#include <string>
#include <vector>

namespace fontconfig
{

class FONTINFO
{
public:
    FONTINFO( std::string aFile, std::string aStyle, std::string aFamily ) :
            m_file( std::move( aFile ) ),
            m_style( std::move( aStyle ) ),
            m_family( std::move( aFamily ) )
    {
    }

    const std::string& File() const   { return m_file; }
    const std::string& Style() const  { return m_style; }
    const std::string& Family() const { return m_family; }

    std::vector<FONTINFO>& Children() { return m_children; }

private:
    std::string           m_file;
    std::string           m_style;
    std::string           m_family;

    std::vector<FONTINFO> m_children;
};

} // namespace fontconfig

#endif //FONT_FONTINFO_H
