#ifndef KICAD_FONTCONFIG_H
#define KICAD_FONTCONFIG_H

#include <Fontconfig/fontconfig.h>

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <fontinfo.hxx>

class REPORTER;
namespace fontconfig
{

struct FONTCONFIG_PAT;

class FONTCONFIG
{
public:
    FONTCONFIG();

    static std::string Version();

    enum class FF_RESULT
    {
        FF_OK,
        FF_ERROR,
        FF_SUBSTITUTE,
        FF_MISSING_BOLD,
        FF_MISSING_ITAL,
        FF_MISSING_BOLD_ITAL
    };

    /**
     * Given a fully-qualified font name ("Times:Bold:Italic") find the closest matching font
     * and return its filepath in \a aFontFile.
     *
     * A return value of false indicates a serious error in the font system.
     */
    FF_RESULT FindFont(const std::string& aFontName, std::string& aFontFile, int& aFaceIndex, bool aBold, bool aItalic,
                       const std::vector<std::string>* aEmbeddedFiles = nullptr);

    /**
     * List the current available font families.
     *
     * @param aDesiredLang The desired language of font name to report back if available,
     *                     otherwise it will fallback.
     * @param aEmbeddedFiles A list of embedded to use for searching fonts, if nullptr, this
     *                       is not used
     * @param aForce If true, force rebuilding the font cache
     */
    void ListFonts( std::vector<std::string>& aFonts, const std::string& aDesiredLang,
                   const std::vector<std::string>* aEmbeddedFiles = nullptr, bool aForce = false);

    /**
     * Set the reporter to use for reporting font substitution warnings.
     *
     * @param aReporter The reporter to use for reporting font substitution warnings.
     */
    static void SetReporter( REPORTER* aReporter );

    /**
     * Get the current reporter used for font substitution warnings.
     *
     * @return The current reporter, or nullptr if not set.
     */
    static REPORTER* GetReporter();

private:
    std::map<std::string, FONTINFO> m_fontInfoCache;
    std::string                     m_fontCacheLastLang;
    static REPORTER*                s_reporter;

    /**
     * Match two rfc 3306 language entries, used for when searching for matching family names
     *
     * The overall logic is simple, either both language tags matched exactly or one tag is
     * "single" level that the other language tag contains.  There are nuances to language tags
     * beyond this but font tags will most likely never be more complex than say "zh-CN" or
     * single tag "en".
     *
     * @param aSearchLang the language being searched for
     * @param aSupportedLang the language being offered
     */
    bool isLanguageMatch(const std::string& aSearchLang, const std::string& aSupportedLang);

    /**
     * Get a list of all family name strings mapped to lang
     *
     * @param aPat reference to FcPattern container
     * @param aFamStringMap Map to be populated with key, value pairs representing lang to
     *                      family name
     */
    void getAllFamilyStrings( FONTCONFIG_PAT&                               aPat,
                              std::unordered_map<std::string, std::string>& aFamStringMap );

    /**
     * Get a family name based on desired language.
     *
     * This will fallback to English or first available string if no language matching string
     * is found.
     *
     * @param aPat reference to FcPattern container
     * @param aDesiredLang Language to research for (RFC3066 format)
     */
    std::string getFamilyStringByLang(FONTCONFIG_PAT& APat, const std::string& aDesiredLang);

    /**
     * Wrapper of FcPatternGetString to return a std::string
     *
     * @param aPat reference to FcPattern container
     * @param aObj The fontconfig property object like FC_FAMILY, FC_STYLE, etc
     * @param aIdx The ith value associated with the property object
     */
    std::string getFcString( FONTCONFIG_PAT& aPat, const char* aObj, int aIdx );
};

} // namespace fontconfig


fontconfig::FONTCONFIG* Fontconfig();


#endif //KICAD_FONTCONFIG_H
