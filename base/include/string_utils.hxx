#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <algorithm>
#include <string>
#include <vector>

void ConvertMarkdown2Html( const std::string& aMarkdownInput, std::string& aHtmlOutput );

/**
 * Convert the old `~...~` overbar notation to the new `~{...}` one.
 */
std::string ConvertToNewOverbarNotation( const std::string& aOldStr );

/**
 * Convert curly quotes and em/en dashes to straight quotes and dashes.
 *
 * @return true if any characters required conversion.
 */
bool ConvertSmartQuotesAndDashes( std::string* aString );

/**
 * Escape/Unescape routines to safely encode reserved-characters in various contexts.
 */
enum ESCAPE_CONTEXT
{
    CTX_NETNAME,
    CTX_LIBID,
    CTX_LEGACY_LIBID,
    CTX_IPC,
    CTX_QUOTED_STR,
    CTX_JS_STR,
    CTX_LINE,
    CTX_CSV,
    CTX_FILENAME,
    CTX_NO_SPACE        // to replace spaces in names that do not accept spaces
};

/**
 * The Escape/Unescape routines use HTML-entity-reference-style encoding to handle
 * characters which are:
 *   (a) not legal in filenames
 *   (b) used as control characters in LIB_IDs
 *   (c) used to delineate hierarchical paths
 */
std::string EscapeString( const std::string& aSource, ESCAPE_CONTEXT aContext );

std::string UnescapeString( const std::string& aSource );

/**
 * Remove markup (such as overbar or subscript) that we can't render to menu items.
 */
std::string PrettyPrintForMenu( const std::string& aString );

/**
 * Capitalize the first letter in each word.
 */
std::string TitleCaps( const std::string& aString );

/**
 * Capitalize only the first word.
 */
std::string InitialCaps( const std::string& aString );

/**
 * Copy bytes from @a aSource delimited string segment to @a aDest buffer.
 *
 * The extracted string will be null terminated even if truncation is necessary
 * because aDestSize was not large enough.
 *
 * @param aDest is the destination byte buffer.
 * @param aSource is the source bytes as a C string.
 * @param aDestSize is the size of the destination byte buffer.
 * @return the number of bytes read from source, which may be more than the number copied,
 *         due to escaping of double quotes and the escape byte itself.
 * @deprecated should use the one which fetches a std::string, below.
 */
int ReadDelimitedText( char* aDest, const char* aSource, int aDestSize );

/**
 * Copy bytes from @a aSource delimited string segment to @a aDest std::string.
 *
 * @param aDest is the destination std::string.
 * @param aSource is the source C string holding utf8 encoded bytes.
 * @return the number of bytes read from source, which may be more than the number copied,
 *         due to escaping of double quotes and the escape byte itself.
 */
int ReadDelimitedText( std::string* aDest, const char* aSource );

/**
 * Return an 8 bit UTF8 string given aString in Unicode form.
 *
 * Any double quoted or back slashes are prefixed with a '\\' byte and the form
 * of this UTF8 byte string is compatible with function ReadDelimitedText().
 *
 * @param aString is the input string to convert.
 * @return the escaped input text, without the wrapping double quotes.
 */
std::string EscapedUTF8( const std::string& aString );

/**
 * Return a new std::string escaped for embedding in HTML.
 */
std::string EscapeHTML( const std::string& aString );

/**
 * Return a new std::string unescaped from HTML format.
 */
std::string UnescapeHTML( const std::string& aString );

/**
 * Removes HTML tags from a string.
 *
 * Do not use for filtering potentially malicious inputs and rendering as HTML
 * without escaping.
 */
std::string RemoveHTMLTags( const std::string& aInput );

/**
 * Wraps links in HTML <a href=""></a> tags.
 */
std::string LinkifyHTML( std::string aStr );

/**
 * Performs a URL sniff-test on a string.
 */
bool IsURL( std::string aStr );

/**
 * Read one line line from \a aFile.
 *
 * @return a pointer the first useful line read by eliminating blank lines and comments.
 */
char* GetLine( FILE* aFile, char* Line, int* LineNum = nullptr, int SizeLine = 255 );

/**
 * Return true if the string is empty or contains only whitespace.
 */
bool NoPrintableChars( const std::string& aString );

/**
 * Return the number of printable (ie: non-formatting) chars.  Used to approximate rendered
 * text size when speed is more important than accuracy.
 */
int PrintableCharCount( const std::string& aString );

/**
 * Remove leading and training spaces, tabs and end of line chars in \a text
 *
 * @return a pointer on the first n char in text
 */
char* StrPurge( char* text );

/**
 * @return a string giving the current date and time.
 */
std::string GetISO8601CurrentDateTime();

/**
 * Compare two strings with alphanumerical content.
 *
 * This function is equivalent to strncmp() or strncasecmp() if \a aIgnoreCase is true
 * except that strings containing numbers are compared by their integer value not
 * by their ASCII code.  In other words U10 would be greater than U2.
 *
 * @param aString1 A std::string reference to the reference string.
 * @param aString2 A std::string reference to the comparison string.
 * @param aIgnoreCase Use true to make the comparison case insensitive.
 * @return An integer value of -1 if \a aString1 is less than \a aString2, 0 if
 *         \a aString1 is equal to \a aString2, or 1 if \a aString1 is greater
 *         than \a aString2.
 */
int StrNumCmp( const std::string& aString1, const std::string& aString2,
                            bool aIgnoreCase = false );


enum class CASE_SENSITIVITY
{
    SENSITIVE,
    INSENSITIVE
};


/**
 * Sort a container of std::string objects, in place, using the StrNumCmp() function.
 */
template <typename T>
inline void StrNumSort( T& aList, CASE_SENSITIVITY aCaseSensitivity )
{
    std::sort( aList.begin(), aList.end(),
               [aCaseSensitivity]( const std::string& lhs, const std::string& rhs )
               {
                   return StrNumCmp( lhs, rhs, aCaseSensitivity == CASE_SENSITIVITY::INSENSITIVE ) < 0;
               } );
}


/**
 * Compare a string against wild card (* and ?) pattern using the usual rules.
 *
 * @return true if pattern matched otherwise false.
 */
bool WildCompareString( const std::string& pattern,
                        const std::string& string_to_tst,
                        bool            case_sensitive = true );

/**
 * Compare strings like the strcmp function but handle numbers and modifiers within the
 * string text correctly for sorting.  eg. 1mF > 55uF
 *
 * @return -1 if first string is less than the second, 0 if the strings are equal, or
 *          1 if the first string is greater than the second.
 */
int ValueStringCompare( const std::string& strFWord, const std::string& strSWord );

/**
 * Break a string into three parts: he alphabetic preamble, the numeric part, and any
 * alphabetic ending.
 *
 * For example C10A is split to C 10 A
 */
int SplitString( const std::string& strToSplit,
                 std::string* strBeginning,
                 std::string* strDigits,
                 std::string* strEnd );

/**
 * Gets the trailing int, if any, from a string.
 *
 * @param aStr the string to check.
 * @return the trailing int or 0 if none found.
 */
int GetTrailingInt( const std::string& aStr );

/**
 * @return a std::string object containing the illegal file name characters for all platforms.
 */
std::string GetIllegalFileNameWxChars();

/**
 * Checks if a full filename is valid, i.e. does not contains illegal chars
 * path separators are allowed
 * @return true if OK.
 */
bool IsFullFileNameValid( const std::string& aFullFilename );

/**
 * Checks \a aName for illegal file name characters.
 *
 * The Windows (DOS) file system forbidden characters already include the forbidden file
 * name characters for both Posix and OSX systems.  The characters \/?*|"\<\> are illegal
 * and are replaced with %xx where xx the hexadecimal equivalent of the replaced character.
 * This replacement may not be as elegant as using an underscore ('_') or hyphen ('-') but
 * it guarantees that there will be no naming conflicts when fixing footprint library names.
 * however, if aReplaceChar is given, it will replace the illegal chars
 *
 * @param aName is a point to a std::string object containing the footprint name to verify.
 * @param aReplaceChar (if not 0) is the replacement char.
 * @return true if any characters have been replaced in \a aName.
 */
bool ReplaceIllegalFileNameChars( std::string& aName, int aReplaceChar = 0 );


/**
 * A helper for sorting strings from the rear.
 *
 * Useful for things like 3D model names where they tend to be largely repetitious at the front.
 */
struct ResortString
{
    bool operator() ( const std::string& strA, const std::string& strB ) const
    {
        std::string::const_reverse_iterator sA = strA.rbegin();
        std::string::const_reverse_iterator eA = strA.rend();

        std::string::const_reverse_iterator sB = strB.rbegin();
        std::string::const_reverse_iterator eB = strB.rend();

        if( strA.empty() )
        {
            if( strB.empty() )
                return false;

            // note: this rule implies that a null string is first in the sort order
            return true;
        }

        if( strB.empty() )
            return false;

        while( sA != eA && sB != eB )
        {
            if( ( *sA ) == ( *sB ) )
            {
                ++sA;
                ++sB;
                continue;
            }

            if( ( *sA ) < ( *sB ) )
                return true;
            else
                return false;
        }

        if( sB == eB )
            return false;

        return true;
    }
};

/**
 * Split the input string into a vector of output strings
 *
 * @note Multiple delimiters are considered to be separate records with empty strings
 *
 * @param aStr Input string with 0 or more delimiters.
 * @param aDelim The string of delimiter.  Multiple characters here denote alternate delimiters.
 * @return a vector of strings
 */
static inline std::vector<std::string> split( const std::string& aStr, const std::string& aDelim )
{
    size_t pos = 0;
    size_t last_pos = 0;
    size_t len;

    std::vector<std::string> tokens;

    while( pos < aStr.size() )
    {
        pos = aStr.find_first_of( aDelim, last_pos );

        if( pos == std::string::npos )
            pos = aStr.size();

        len = pos - last_pos;

        tokens.push_back( aStr.substr( last_pos, len ) );

        last_pos = pos + 1;
    }

    return tokens;
}

/// Utility to build comma separated lists in messages
inline void AccumulateDescription( std::string& aDesc, const std::string& aItem )
{
    if( !aDesc.empty() )
        aDesc += ", ";

    aDesc += aItem;
}


/**
 * Build a comma-separated list from a collection of strings.
 * (e.g. std::vector, wxArrayString, etc).
 */
template <typename T>
inline void AccumulateDescriptions( std::string& aDesc, const T& aItemCollection )
{
    for( const auto& item : aItemCollection )
        AccumulateDescription( aDesc, item );
}


template <typename T>
inline std::string AccumulateDescriptions( const T& aItemCollection )
{
    std::string desc;
    AccumulateDescriptions( desc, aItemCollection );
    return desc;
}

/**
 * Split \a aString to a string list separated at \a aSplitter.
 *
 * @param aText is the text to split.
 * @param aStrings will contain the split lines.
 * @param aSplitter is the 'split' character.
 */
void StringSplit(const std::string& aText, std::vector<std::string>& aStrings, char aSplitter);

/**
 * Remove trailing zeros from a string containing a converted float number.
 *
 * The trailing zeros are removed if the mantissa has more than \a aTrailingZeroAllowed
 * digits and some trailing zeros.
 */
void StripTrailingZeros( std::string& aStringValue, unsigned aTrailingZeroAllowed = 1 );

/**
 * Print a float number without using scientific notation and no trailing 0
 * We want to avoid scientific notation in S-expr files (not easy to read)
 * for floating numbers.
 *
 * We cannot always just use the %g or the %f format to print a fp number
 * this helper function uses the %f format when needed, or %g when %f is
 * not well working and then removes trailing 0
 */
std::string UIDouble2Str( double aValue );

/**
 * Print a float number without using scientific notation and no trailing 0
 * This function is intended in uses to write to file, it ignores locale
 *
 * We cannot always just use the %g or the %f format to print a fp number
 * this helper function uses the %f format when needed, or %g when %f is
 * not well working and then removes trailing 0
 */
std::string FormatDouble2Str( double aValue );

/**
 * Convert a std::string to a UTF8 encoded C string for all wxWidgets build modes.
 *
 * std::string is a std::string, not a wxT() or _().  The scope of the return value
 * is very limited and volatile, but can be used with printf() style functions well.
 *
 * @note Trying to convert it to a function is tricky because of the type of the
 *       parameter!
 */
#define TO_UTF8( str ) ( (const char*) ( str ).data() )

/**
 * Convert an expected UTF8 encoded std::string to a std::string.
 * If fails, try to convert using current locale
 * If still fails, return the initial string (can be already a converted string)
 */
std::string From_UTF8( const std::string& aString );
std::string  From_UTF8( const char* cstring );

/**
 * Normalize file path \a aFileUri to URI convention.
 *
 * Unfortunately none of the wxWidgets objects results in acceptable file URIs which breaks
 * PDF plotting URI links.  This is an attempt to normalize Windows local file paths to a
 * URI that PDF readers that can run JavaScript can handle.
 *
 * @note This does not expand environment or user variables.  Variable expansion should be
 *       performed before calling.  If \a aFileUri does not begin with 'file://', \a aFileUri
 *       returned unchanged.
 *
 * @param aFileUri is the string to be normalized.
 * @return the normalized string.
 */
std::string NormalizeFileUri( const std::string& aFileUri );

/**
 * Expand stacked pin notation like [1,2,3], [1-4], [A1-A4], or [AA1-AA3,AB4,CD12-CD14]
 * into individual pin numbers, supporting both numeric and alphanumeric pin prefixes.
 *
 * Examples:
 *   "[1,2,3]" -> {"1", "2", "3"}
 *   "[1-4]" -> {"1", "2", "3", "4"}
 *   "[A1-A3]" -> {"A1", "A2", "A3"}
 *   "[AA1-AA3,AB4]" -> {"AA1", "AA2", "AA3", "AB4"}
 *   "5" -> {"5"} (non-bracketed pins returned as-is)
 *
 * @param aPinName is the pin name to expand (may or may not use stacked notation)
 * @param aValid is optionally set to indicate whether the notation was valid
 * @return vector of individual pin numbers
 */
std::vector<std::string> ExpandStackedPinNotation( const std::string& aPinName,
                                                            bool* aValid = nullptr );

/**
 * Count the number of pins represented by stacked pin notation without allocating strings.
 *
 * This is a fast alternative to ExpandStackedPinNotation().size() for cases where only
 * the count is needed.
 *
 * @param aPinName is the pin name to count (may or may not use stacked notation)
 * @param aValid is optionally set to indicate whether the notation was valid
 * @return count of individual pins represented (always >= 1)
 */
int CountStackedPinNotation( const std::string& aPinName, bool* aValid = nullptr );


std::string GetDefaultVariantName();

int SortVariantNames( const std::string& aLhs, const std::string& aRhs );

struct LOAD_MESSAGE;

/**
 * Parse library load error messages, extracting user-facing information while
 * stripping internal code locations.
 *
 * @param aErrorString is the raw error string from GetLibraryLoadErrors()
 * @param aSeverity is the severity to assign to all extracted messages
 * @return vector of LOAD_MESSAGE with cleaned error text
 */
std::vector<LOAD_MESSAGE> ExtractLibraryLoadErrors( const std::string& aErrorString,
                                                                  int aSeverity );

#endif  // STRING_UTILS_H
