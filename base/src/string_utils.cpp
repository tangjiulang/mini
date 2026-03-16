/**
 * @file string_utils.cpp
 * @brief Some useful functions to handle strings.
 */

#include <clocale>
#include <cmath>
#include <regex>
#include <map>
#include <map_helpers.hxx>
#include <fmt/core.h>
#include <string_utils.hxx>
#include <severity.hxx>
#include <fmt/chrono.h>


/**
 * Illegal file name characters used to ensure file names will be valid on all supported
 * platforms.  This is the list of illegal file name characters for Windows which includes
 * the illegal file name characters for Linux and OSX.
 */
static constexpr std::string_view illegalFileNameChars = "\\/:\"<>|*?";

static const char defaultVariantName[] = "< Default >";


// Checks if a full filename is valid, i.e. does not contains illegal chars
std::string GetForbiddenFileNameChars()
{
#ifdef _WIN32
    // Windows 禁止字符:  < > : " / \ | ? *
    return "<>:\"|?*";
#else
    // Linux/macOS 禁止字符:  /
    return "/";
#endif
}

bool IsFullFileNameValid(const std::string& aFullFilename)
{
    if(aFullFilename.empty())
        return false;

    // 先创建一个副本用于检查
    std::string filtered_fullpath = aFullFilename;

#ifdef _WIN32
    // 在 Windows 下，将允许的路径分隔符替换掉
    std::replace(filtered_fullpath.begin(), filtered_fullpath.end(), '/', '_');
    std::replace(filtered_fullpath.begin(), filtered_fullpath.end(), '\\', '_');

    // 如果是盘符开头，如 "C:\..."，允许第二个字符为 ':'
    if(filtered_fullpath.size() > 1 && filtered_fullpath[1] == ':')
        filtered_fullpath[1] = ' ';
#endif

    // 检查是否包含禁止字符
    const std::string forbidden = GetForbiddenFileNameChars();
    if(filtered_fullpath.find_first_of(forbidden) != std::string::npos)
        return false;

    return true;
}

std::string ConvertToNewOverbarNotation(const std::string& aOldStr)
{
    std::string newStr;
    bool     inOverbar = false;

    // Don't get tripped up by the legacy empty-string token.
    if(aOldStr == "~")
        return aOldStr;

    newStr.reserve(aOldStr.length());

    for(std::string::const_iterator chIt = aOldStr.begin(); chIt != aOldStr.end(); ++chIt)
    {
        if(*chIt == '~')
        {
            std::string::const_iterator lookahead = chIt + 1;

            if(lookahead != aOldStr.end() && *lookahead == '~')
            {
                if(++lookahead != aOldStr.end() && *lookahead == '{')
                {
                    // This way the subsequent opening curly brace will not start an
                    // overbar.
                    newStr += "~~{}";
                    continue;
                }

                // Two subsequent tildes mean a tilde.
                newStr += "~";
                ++chIt;
                continue;
            }
            else if(lookahead != aOldStr.end() && *lookahead == '{')
            {
                // Could mean the user wants "{" with an overbar, but more likely this
                // is a case of double notation conversion.  Bail out.
                return aOldStr;
            }
            else
            {
                if(inOverbar)
                {
                    newStr += "}";
                    inOverbar = false;
                }
                else
                {
                    newStr += "~{";
                    inOverbar = true;
                }

                continue;
            }
        }
        else if((*chIt == ' ' || *chIt == '}' || *chIt == ')') && inOverbar)
        {
            // Spaces were used to terminate overbar as well
            newStr += "}";
            inOverbar = false;
        }

        newStr + *chIt;
    }

    // Explicitly end the overbar even if there was no terminating '~' in the aOldStr.
    if(inOverbar)
        newStr += "}";

    return newStr;
}


bool ConvertSmartQuotesAndDashes(std::string* aString)
{
    bool retVal = false;

    for(std::string::iterator ii = aString->begin(); ii != aString->end(); ++ii)
    {
        if(*ii == L'\u2018' || *ii == L'\u2019')
        {
            *ii = '\'';
            retVal = true;
        }
        if(*ii == L'\u201C' || *ii == L'\u201D')
        {
            *ii = '"';
            retVal = true;
        }
        if(*ii == L'\u2013' || *ii == L'\u2014')
        {
            *ii = '-';
            retVal = true;
        }
    }

    return retVal;
}


std::string EscapeString(const std::string& aSource, ESCAPE_CONTEXT aContext)
{
    std::string          converted;
    std::vector<bool> braceStack; // true == formatting construct

    converted.reserve(aSource.length());

    for(char c : aSource)
    {
        if(aContext == CTX_NETNAME)
        {
            if(c == '/')
                converted += "{slash}";
            else if(c == '\n' || c == '\r')
                converted += std::string(); // drop
            else
                converted += c;
        }
        else if(aContext == CTX_LIBID || aContext == CTX_LEGACY_LIBID)
        {
            // We no longer escape '/' in LIB_IDs, but we used to
            if(c == '/' && aContext == CTX_LEGACY_LIBID)
                converted += "{slash}";
            else if(c == '\\')
                converted += "{backslash}";
            else if(c == '<')
                converted += "{lt}";
            else if(c == '>')
                converted += "{gt}";
            else if(c == ':')
                converted += "{colon}";
            else if(c == '\"')
                converted += "{dblquote}";
            else if(c == '\n' || c == '\r')
                converted += std::string(); // drop
            else
                converted += c;
        }
        else if(aContext == CTX_IPC)
        {
            if(c == '/')
                converted += "{slash}";
            else if(c == ',')
                converted += "{comma}";
            else if(c == '\"')
                converted += "{dblquote}";
            else
                converted += c;
        }
        else if(aContext == CTX_QUOTED_STR)
        {
            if(c == '\"')
                converted += "{dblquote}";
            else
                converted += c;
        }
        else if(aContext == CTX_JS_STR)
        {
            if(c >= 0x7F || c == '\'' || c == '"' || c == '\\' || c == '(' || c == ')')
            {
                unsigned int code = c;
                char         buffer[16];
                snprintf(buffer, sizeof(buffer), "\\u%4.4X", code);
                converted += buffer;
            }
            else
            {
                converted += c;
            }
        }
        else if(aContext == CTX_LINE)
        {
            if(c == '\n' || c == '\r')
                converted += "{return}";
            else
                converted += c;
        }
        else if(aContext == CTX_FILENAME)
        {
            if(c == '/')
                converted += "{slash}";
            else if(c == '\\')
                converted += "{backslash}";
            else if(c == '\"')
                converted += "{dblquote}";
            else if(c == '<')
                converted += "{lt}";
            else if(c == '>')
                converted += "{gt}";
            else if(c == '|')
                converted += "{bar}";
            else if(c == ':')
                converted += "{colon}";
            else if(c == '\t')
                converted += "{tab}";
            else if(c == '\n' || c == '\r')
                converted += "{return}";
            else
                converted += c;
        }
        else if(aContext == CTX_NO_SPACE)
        {
            if(c == ' ')
                converted += "{space}";
            else
                converted += c;
        }
        else if(aContext == CTX_CSV)
        {
            if(c == ',')
                converted += "{comma}";
            else if(c == '\n' || c == '\r')
                converted += "{return}";
            else
                converted += c;
        }
        else
        {
            converted += c;
        }
    }

    return converted;
}


std::string UnescapeString(const std::string& aSource)
{
    size_t sourceLen = aSource.length();

    // smallest escape string is three characters, shortcut everything else
    if(sourceLen <= 2)
    {
        return aSource;
    }

    std::string newbuf;
    newbuf.reserve(sourceLen);

    char prev = 0;
    char ch = 0;

    for(size_t i = 0; i < sourceLen; ++i)
    {
        prev = ch;
        ch = aSource[i];

        if(ch == '{')
        {
            std::string token;
            int      depth = 1;
            bool     terminated = false;

            for(i = i + 1; i < sourceLen; ++i)
            {
                ch = aSource[i];

                if(ch == '{')
                    depth++;
                else if(ch == '}')
                    depth--;

                if(depth <= 0)
                {
                    terminated = true;
                    break;
                }
                else
                {
                    token += ch;
                }
            }

            if(!terminated)
            {
                newbuf += std::string("{" + UnescapeString(token));
            }
            else if(prev == '$' || prev == '~' || prev == '^' || prev == '_')
            {
                newbuf += std::string("{" + UnescapeString(token) + "}");
            }
            else if(token == std::string("dblquote"))
                newbuf += "\"";
            else if(token == std::string("quote"))
                newbuf += ("'");
            else if(token == std::string("lt"))
                newbuf += ("<");
            else if(token == std::string("gt"))
                newbuf += (">");
            else if(token == std::string("backslash"))
                newbuf += ("\\");
            else if(token == std::string("slash"))
                newbuf += ("/");
            else if(token == std::string("bar"))
                newbuf += ("|");
            else if(token == std::string("comma"))
                newbuf += (",");
            else if(token == std::string("colon"))
                newbuf += (":");
            else if(token == std::string("space"))
                newbuf += (" ");
            else if(token == std::string("dollar"))
                newbuf += ("$");
            else if(token == std::string("tab"))
                newbuf += ("\t");
            else if(token == std::string("return"))
                newbuf += ("\n");
            else if(token == std::string("brace"))
                newbuf += ("{");
            else
            {
                newbuf += std::string(("{") + UnescapeString(token) + ("}"));
            }
        }
        else
        {
            newbuf += ch;
        }
    }

    return newbuf;
}


std::string TitleCaps(const std::string& aString)
{
    std::vector<std::string> words;
    std::string      result;

    StringSplit(aString, words, ' ');

    result.reserve(aString.length());

    for(std::string& word : words)
    {
        if(!result.empty())
            result += " ";
        if(!word.empty())
            word[0] = std::toupper(word[0]);
        result += word;
    }

    return result;
}


std::string InitialCaps(const std::string& aString)
{
    std::vector<std::string> words;
    std::string      result;

    StringSplit(aString, words, ' ');

    result.reserve(aString.length());

    for(std::string& word : words)
    {
        if(result.empty())
        {
            if(!word.empty())
                word[0] = std::toupper(word[0]);
            result += word;
        }
        else
        {
            std::transform(word.begin(), word.end(), word.begin(),
                           [](unsigned char c)
                           {
                               return std::tolower(c);
                           });
            result += " " + word;
        }
    }

    return result;
}


int ReadDelimitedText(std::string* aDest, const char* aSource)
{
    std::string utf8; // utf8 but without escapes and quotes.
    bool        inside = false;
    const char* start = aSource;
    char        cc;

    while((cc = *aSource++) != 0)
    {
        if(cc == '"')
        {
            if(inside)
                break; // 2nd double quote is end of delimited text

            inside = true; // first delimiter found, make note, do not copy
        }

        else if(inside)
        {
            if(cc == '\\')
            {
                cc = *aSource++;

                if(!cc)
                    break;

                // do no copy the escape byte if it is followed by \ or "
                if(cc != '"' && cc != '\\')
                    utf8 += '\\';

                utf8 += cc;
            }
            else
            {
                utf8 += cc;
            }
        }
    }

    *aDest = From_UTF8(utf8.c_str());

    return aSource - start;
}


int ReadDelimitedText(char* aDest, const char* aSource, int aDestSize)
{
    if(aDestSize <= 0)
        return 0;

    bool        inside = false;
    const char* start = aSource;
    char*       limit = aDest + aDestSize - 1;
    char        cc;

    while((cc = *aSource++) != 0 && aDest < limit)
    {
        if(cc == '"')
        {
            if(inside)
                break; // 2nd double quote is end of delimited text

            inside = true; // first delimiter found, make note, do not copy
        }
        else if(inside)
        {
            if(cc == '\\')
            {
                cc = *aSource++;

                if(!cc)
                    break;

                // do no copy the escape byte if it is followed by \ or "
                if(cc != '"' && cc != '\\')
                    *aDest++ = '\\';

                if(aDest < limit)
                    *aDest++ = cc;
            }
            else
            {
                *aDest++ = cc;
            }
        }
    }

    *aDest = 0;

    return aSource - start;
}

size_t ReplaceAll(std::string& s, const std::string& from, const std::string& to)
{
    size_t count = 0;
    size_t pos = 0;

    while((pos = s.find(from, pos)) != std::string::npos)
    {
        s.replace(pos, from.length(), to);
        pos += to.length();
        count++;
    }

    return count;
}

std::string EscapedUTF8(const std::string& aString)
{
    std::string str = aString;

    // No new-lines allowed in quoted strings
    ReplaceAll(str, "\r\n", "\r");
    ReplaceAll(str, "\n", "\r");

    std::string utf8 = TO_UTF8(aString);

    std::string ret;

    ret.reserve(utf8.length() + 2);

    ret += '"';

    for(std::string::const_iterator it = utf8.begin(); it != utf8.end(); ++it)
    {
        // this escaping strategy is designed to be compatible with ReadDelimitedText():
        if(*it == '"')
        {
            ret += '\\';
            ret += '"';
        }
        else if(*it == '\\')
        {
            ret += '\\'; // double it up
            ret += '\\';
        }
        else
        {
            ret += *it;
        }
    }

    ret += '"';

    return ret;
}


std::string EscapeHTML(const std::string& aString)
{
    std::string converted;

    converted.reserve(aString.length());

    for(char c : aString)
    {
        if(c == '\"')
            converted += "&quot;";
        else if(c == '\'')
            converted += "&apos;";
        else if(c == '&')
            converted += ("&amp;");
        else if(c == '<')
            converted += ("&lt;");
        else if(c == '>')
            converted += ("&gt;");
        else
            converted += c;
    }

    return converted;
}


std::string UnescapeHTML(const std::string& input)
{
    static const std::map<std::string, std::string> replacements = {
        { "quot", "\"" }, { "apos", "'" }, { "amp", "&" }, { "lt", "<" }, { "gt", ">" }
    };

    std::regex re("&(#(\\d+)|#x([0-9a-fA-F]+)|quot|apos|amp|lt|gt);");

    std::string          result;
    std::sregex_iterator it(input.begin(), input.end(), re);
    std::sregex_iterator end;

    size_t last = 0;

    for(; it != end; ++it)
    {
        const std::smatch& m = *it;

        result.append(input.substr(last, m.position() - last));

        std::string code = m[1];
        std::string dec = m[2];
        std::string hex = m[3];

        if(!dec.empty() || !hex.empty())
        {
            unsigned long val = 0;

            if(!dec.empty())
                val = std::stoul(dec);
            else
                val = std::stoul(hex, nullptr, 16);

            if(val != 0)
                result += static_cast<char>(val);
        }
        else
        {
            auto it = replacements.find(code);
            if(it != replacements.end())
                result += it->second;
        }

        last = m.position() + m.length();
    }

    result.append(input.substr(last));

    return result;
}


std::string RemoveHTMLTags(const std::string& input)
{
    static const std::regex re("<[^>]*>");
    return std::regex_replace(input, re, "");
}


std::string LinkifyHTML(std::string str)
{
    static const std::regex re(R"(\b(https?|ftp|file)://([-\w+&@#/%?=~|!:,.;]*[^.,:;<>\(\)\s¶]))", std::regex::icase);

    return std::regex_replace(str, re, "<a href=\"$&\">$&</a>");
}


bool IsURL(const std::string& str)
{
    static const std::regex re(R"((https?|ftp|file)://([-\w+&@#/%?=~|!:,.;]*[^.,:;<>\s¶]))", std::regex::icase);

    return std::regex_search(str, re);
}


bool NoPrintableChars(const std::string& s)
{
    return std::all_of(s.begin(), s.end(),
                       [](unsigned char c)
                       {
                           return std::isspace(c);
                       });
}

int PrintableCharCount(const std::string& aString)
{
    int char_count = 0;
    int overbarDepth = -1;
    int superSubDepth = -1;
    int braceNesting = 0;

    for(auto chIt = aString.begin(), end = aString.end(); chIt < end; ++chIt)
    {
        if(*chIt == '\t')
        {
            // We don't format tabs in bitmap text (where this is currently used), so just
            // drop them from the count.
            continue;
        }
        else if(*chIt == '^' && superSubDepth == -1)
        {
            auto lookahead = chIt;

            if(++lookahead != end && *lookahead == '{')
            {
                chIt = lookahead;
                superSubDepth = braceNesting;
                braceNesting++;
                continue;
            }
        }
        else if(*chIt == '_' && superSubDepth == -1)
        {
            auto lookahead = chIt;

            if(++lookahead != end && *lookahead == '{')
            {
                chIt = lookahead;
                superSubDepth = braceNesting;
                braceNesting++;
                continue;
            }
        }
        else if(*chIt == '~' && overbarDepth == -1)
        {
            auto lookahead = chIt;

            if(++lookahead != end && *lookahead == '{')
            {
                chIt = lookahead;
                overbarDepth = braceNesting;
                braceNesting++;
                continue;
            }
        }
        else if(*chIt == '{')
        {
            braceNesting++;
        }
        else if(*chIt == '}')
        {
            if(braceNesting > 0)
                braceNesting--;

            if(braceNesting == superSubDepth)
            {
                superSubDepth = -1;
                continue;
            }

            if(braceNesting == overbarDepth)
            {
                overbarDepth = -1;
                continue;
            }
        }

        char_count++;
    }

    return char_count;
}


char* StrPurge(char* text)
{
    static const char whitespace[] = " \t\n\r\f\v";

    if(text)
    {
        while(*text && strchr(whitespace, *text))
            ++text;

        char* cp = text + strlen(text) - 1;

        while(cp >= text && strchr(whitespace, *cp))
            *cp-- = '\0';
    }

    return text;
}


char* GetLine(FILE* File, char* Line, int* LineNum, int SizeLine)
{
    do
    {
        if(fgets(Line, SizeLine, File) == nullptr)
            return nullptr;

        if(LineNum)
            *LineNum += 1;

    } while(Line[0] == '#' || Line[0] == '\n' || Line[0] == '\r' || Line[0] == 0);

    strtok(Line, "\n\r");
    return Line;
}


std::string GetISO8601CurrentDateTime()
{
    auto now = std::chrono::system_clock::now();
    return std::format("{:%FT%T}", now);
}

int StrNumCmp(const std::string& s1, const std::string& s2, bool ignoreCase)
{
    int nb1 = 0, nb2 = 0;

    auto str1 = s1.begin();
    auto str2 = s2.begin();

    while(str1 != s1.end() && str2 != s2.end())
    {
        char c1 = *str1;
        char c2 = *str2;

        if(std::isdigit((unsigned char) c1) && std::isdigit((unsigned char) c2))
        {
            nb1 = nb2 = 0;

            do
            {
                nb1 = nb1 * 10 + (*str1 - '0');
                ++str1;
            } while(str1 != s1.end() && std::isdigit((unsigned char) *str1));

            do
            {
                nb2 = nb2 * 10 + (*str2 - '0');
                ++str2;
            } while(str2 != s2.end() && std::isdigit((unsigned char) *str2));

            if(nb1 < nb2)
                return -1;
            if(nb1 > nb2)
                return 1;

            c1 = (str1 != s1.end()) ? *str1 : 0;
            c2 = (str2 != s2.end()) ? *str2 : 0;
        }

        if(ignoreCase)
        {
            char uc1 = std::toupper((unsigned char) c1);
            char uc2 = std::toupper((unsigned char) c2);

            if(uc1 != uc2)
                return uc1 < uc2 ? -1 : 1;
        }
        else
        {
            if(c1 < c2)
                return -1;
            if(c1 > c2)
                return 1;
        }

        if(str1 != s1.end())
            ++str1;
        if(str2 != s2.end())
            ++str2;
    }

    if(str1 == s1.end() && str2 != s2.end())
        return -1;
    if(str1 != s1.end() && str2 == s2.end())
        return 1;

    return 0;
}

bool WildCompareString(const std::string& pattern, const std::string& str, bool case_sensitive)
{
    std::string p = pattern;
    std::string s = str;

    if(!case_sensitive)
    {
        std::transform(p.begin(), p.end(), p.begin(), ::toupper);
        std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    }

    const char* wild = p.c_str();
    const char* string = s.c_str();

    const char* cp = nullptr;
    const char* mp = nullptr;

    while(*string && *wild != '*')
    {
        if(*wild != *string && *wild != '?')
            return false;

        wild++;
        string++;
    }

    while(*string)
    {
        if(*wild == '*')
        {
            if(!*++wild)
                return true;

            mp = wild;
            cp = string + 1;
        }
        else if(*wild == *string || *wild == '?')
        {
            wild++;
            string++;
        }
        else
        {
            wild = mp;
            string = cp++;
        }
    }

    while(*wild == '*')
        wild++;

    return !*wild;
}

static std::string trim(const std::string& s)
{
    size_t start = s.find_first_not_of(" \t\n\r");
    size_t end = s.find_last_not_of(" \t\n\r");

    if(start == std::string::npos)
        return "";

    return s.substr(start, end - start + 1);
}

bool ApplyModifier(double& value, const std::string& str)
{
    static const std::string modifiers = "afpnuµμmLRFkKMGTPE";

    if(str.empty())
        return false;

    char        modifier = ' ';
    std::string units;

    if(modifiers.find(str[0]) != std::string::npos)
    {
        modifier = str[0];
        units = trim(str.substr(1));
    }
    else
    {
        units = trim(str);
    }

    if(!units.empty())
    {
        std::string u = units;
        std::transform(u.begin(), u.end(), u.begin(), ::tolower);

        if(u != "f" && u != "hz" && u != "w" && u != "v" && u != "a" && u != "h")
            return false;
    }

    if(modifier == 'a')
        value *= 1e-18;
    else if(modifier == 'f')
        value *= 1e-15;
    else if(modifier == 'p')
        value *= 1e-12;
    else if(modifier == 'n')
        value *= 1e-9;
    else if(modifier == 'u' || modifier == std::string("µ")[0] || modifier == std::string("μ")[0])
        value *= 1e-6;
    else if(modifier == 'm' || modifier == 'L')
        value *= 1e-3;
    else if(modifier == 'k' || modifier == 'K')
        value *= 1e3;
    else if(modifier == 'M')
        value *= 1e6;
    else if(modifier == 'G')
        value *= 1e9;
    else if(modifier == 'T')
        value *= 1e12;
    else if(modifier == 'P')
        value *= 1e15;
    else if(modifier == 'E')
        value *= 1e18;

    return true;
}

static void replaceAll(std::string& s, const std::string& from, const std::string& to)
{
    size_t pos = 0;

    while((pos = s.find(from, pos)) != std::string::npos)
    {
        s.replace(pos, from.length(), to);
        pos += to.length();
    }
}

static int cmpNoCase(const std::string& a, const std::string& b)
{
    std::string aa = a;
    std::string bb = b;

    std::transform(aa.begin(), aa.end(), aa.begin(), ::tolower);
    std::transform(bb.begin(), bb.end(), bb.begin(), ::tolower);

    if(aa < bb)
        return -1;
    if(aa > bb)
        return 1;
    return 0;
}

bool convertSeparators(std::string* value)
{
    replaceAll(*value, " ", "");

    char ambiguousSeparator = '?';
    char thousandsSeparator = '?';
    bool thousandsSeparatorFound = false;

    char decimalSeparator = '?';
    bool decimalSeparatorFound = false;

    int digits = 0;

    for(int ii = (int) value->length() - 1; ii >= 0; --ii)
    {
        char c = (*value)[ii];

        if(std::isdigit((unsigned char) c))
        {
            digits++;
        }
        else if(c == '.' || c == ',')
        {
            if(decimalSeparator != '?' || thousandsSeparator != '?')
            {
                if(c == decimalSeparator)
                {
                    if(thousandsSeparatorFound || decimalSeparatorFound)
                        return false;

                    decimalSeparatorFound = true;
                }
                else if(c == thousandsSeparator)
                {
                    if(digits != 3)
                        return false;

                    thousandsSeparatorFound = true;
                }
            }
            else if(ambiguousSeparator != '?')
            {
                if(c == ambiguousSeparator)
                {
                    thousandsSeparator = ambiguousSeparator;
                    thousandsSeparatorFound = true;
                    decimalSeparator = (c == '.') ? ',' : '.';
                }
                else
                {
                    decimalSeparator = ambiguousSeparator;
                    decimalSeparatorFound = true;
                    thousandsSeparator = c;
                    thousandsSeparatorFound = true;
                }
            }
            else
            {
                if((ii == 1 && (*value)[0] == '0') || digits != 3)
                {
                    decimalSeparator = c;
                    decimalSeparatorFound = true;
                    thousandsSeparator = (c == '.') ? ',' : '.';
                }
                else
                {
                    ambiguousSeparator = c;
                }
            }

            digits = 0;
        }
        else
        {
            digits = 0;
        }
    }

    if(decimalSeparator == '?' && thousandsSeparator == '?')
    {
        const struct lconv* lc = localeconv();

        decimalSeparator = lc->decimal_point[0];
        thousandsSeparator = (decimalSeparator == '.') ? ',' : '.';
    }

    replaceAll(*value, std::string(1, thousandsSeparator), "");
    replaceAll(*value, std::string(1, decimalSeparator), ".");

    return true;
}


int ValueStringCompare(const std::string& strFWord, const std::string& strSWord)
{
    std::string fWord = UnescapeString(strFWord);
    std::string sWord = UnescapeString(strSWord);

    std::string strFWordBeg, strFWordMid, strFWordEnd;
    std::string strSWordBeg, strSWordMid, strSWordEnd;

    SplitString(fWord, &strFWordBeg, &strFWordMid, &strFWordEnd);
    SplitString(sWord, &strSWordBeg, &strSWordMid, &strSWordEnd);

    int isEqual = cmpNoCase(strFWordBeg, strSWordBeg);

    if(isEqual != 0)
        return isEqual;

    double lFirstNumber = 0;
    double lSecondNumber = 0;
    bool   endingIsModifier = false;

    convertSeparators(&strFWordMid);
    convertSeparators(&strSWordMid);

    if(!strFWordMid.empty())
        lFirstNumber = std::stod(strFWordMid);

    if(!strSWordMid.empty())
        lSecondNumber = std::stod(strSWordMid);

    endingIsModifier |= ApplyModifier(lFirstNumber, strFWordEnd);
    endingIsModifier |= ApplyModifier(lSecondNumber, strSWordEnd);

    if(lFirstNumber > lSecondNumber)
        return 1;
    if(lFirstNumber < lSecondNumber)
        return -1;

    if(!endingIsModifier)
        return cmpNoCase(strFWordEnd, strSWordEnd);

    return 0;
}


int SplitString(const std::string& str, std::string* beg, std::string* digits, std::string* end)
{
    static const std::string separators = ".,";
    char                     infix = 0;

    beg->clear();
    digits->clear();
    end->clear();

    if(str.empty())
        return 0;

    int ii;

    for(ii = str.size() - 1; ii >= 0; --ii)
    {
        if(std::isdigit((unsigned char) str[ii]))
            break;
    }

    if(ii < 0)
    {
        *beg = str;
        return 0;
    }

    *end = str.substr(ii + 1);

    int position = ii + 1;

    for(; ii >= 0; --ii)
    {
        char c = str[ii];

        if(std::isdigit((unsigned char) c))
            continue;

        if(separators.find(c) != std::string::npos)
            continue;

        break;
    }

    if(ii < 0)
        *digits = str.substr(0, position);
    else
    {
        *digits = str.substr(ii + 1, position - ii - 1);
        *beg = str.substr(0, ii + 1);
    }

    if(infix)
    {
        replaceAll(*digits, std::string(1, infix), ".");
        *end = infix + *end;
    }

    return 0;
}

int GetTrailingInt(const std::string& str)
{
    int number = 0;
    int base = 1;

    int index = (int)str.size() - 1;

    while(index >= 0)
    {
        char chr = str[index];

        if(chr < '0' || chr > '9')
            break;

        number += (chr - '0') * base;
        base *= 10;
        --index;
    }

    return number;
}

std::string GetIllegalFileNameWxChars()
{
    return std::string(illegalFileNameChars);
}


bool ReplaceIllegalFileNameChars(std::string& aName, int aReplaceChar)
{
    size_t first_illegal_pos = aName.find_first_of(illegalFileNameChars);

    if(first_illegal_pos == std::string::npos)
    {
        return false;
    }

    std::string result;
    // result will be at least equal to original, add 16 in case of hex replacements
    result.reserve(aName.length() + 16);
    // append the valid part
    result.append(aName, 0, first_illegal_pos);

    for(size_t i = first_illegal_pos; i < aName.length(); ++i)
    {
        char c = aName[i];

        // Check if this specific char is illegal
        if(illegalFileNameChars.find(c) != std::string_view::npos)
        {
            if(aReplaceChar)
            {
                result.push_back(aReplaceChar);
            }
            else
            {
                fmt::format_to(std::back_inserter(result), "%{:02x}", static_cast<unsigned char>(c));
            }
        }
        else
        {
            result.push_back(c);
        }
    }

    aName = std::move(result);
    return true;
}


void StringSplit(const std::string& aText, std::vector<std::string>& aStrings, char aSplitter)
{
    std::string              tmp;

    for(char c : aText)
    {
        if(c == aSplitter)
        {
            aStrings.push_back(tmp);
            tmp.clear();
        }
        else
        {
            tmp += c;
        }
    }

    if(!tmp.empty())
        aStrings.push_back(tmp);

}


void StripTrailingZeros(std::string& strValue, unsigned trailingZeroAllowed)
{
    struct lconv* lc = localeconv();
    char          sep = lc->decimal_point[0];

    size_t sep_pos = strValue.find(sep);

    if(sep_pos != std::string::npos)
    {
        size_t min_len = sep_pos + trailingZeroAllowed + 1;

        while(strValue.size() > min_len)
        {
            if(strValue.back() == '0')
                strValue.pop_back();
            else
                break;
        }
    }
}


std::string FormatDouble2Str(double aValue)
{
    std::string buf;

    if(aValue != 0.0 && std::fabs(aValue) <= 0.0001)
    {
        buf = fmt::format("{:.16f}", aValue);

        // remove trailing zeros (and the decimal marker if needed)
        while(!buf.empty() && buf[buf.size() - 1] == '0')
        {
            buf.pop_back();
        }

        // if the value was really small
        // we may have just stripped all the zeros after the decimal
        if(buf[buf.size() - 1] == '.')
        {
            buf.pop_back();
        }
    }
    else
    {
        buf = fmt::format("{:.10g}", aValue);
    }

    return buf;
}


std::string UIDouble2Str(double aValue)
{
    char buf[50];
    int  len;

    if(aValue != 0.0 && std::fabs(aValue) <= 0.0001)
    {
        // For these small values, %f works fine,
        // and %g gives an exponent
        len = snprintf(buf, sizeof(buf), "%.16f", aValue);

        while(--len > 0 && buf[len] == '0')
            buf[len] = '\0';

        if(buf[len] == '.' || buf[len] == ',')
            buf[len] = '\0';
        else
            ++len;
    }
    else
    {
        // For these values, %g works fine, and sometimes %f
        // gives a bad value (try aValue = 1.222222222222, with %.16f format!)
        len = snprintf(buf, sizeof(buf), "%.10g", aValue);
    }

    return std::string(buf, len);
}


std::string From_UTF8(const char* cstring)
{
    if(!cstring)
        return std::string();

    std::string s(cstring); // 默认直接用 UTF-8 字节存储
    return s;
}

std::string From_UTF8(const std::string& str)
{
    return str; // std::string 默认可以存储 UTF-8
}


std::string NormalizeFileUri(const std::string& fileUri)
{
    const std::string prefix = "file://";
    std::string       pathAndFileName;

    if(fileUri.compare(0, prefix.size(), prefix) == 0)
        pathAndFileName = fileUri.substr(prefix.size());
    else
        pathAndFileName = fileUri; // 不以 file:// 开头，直接用原串

    std::string tmp = pathAndFileName;

    // 替换 '\' -> '/'
    std::replace(tmp.begin(), tmp.end(), '\\', '/');

    // 删除 ':'
    tmp.erase(std::remove(tmp.begin(), tmp.end(), ':'), tmp.end());

    // 确保以 '/' 开头
    if(!tmp.empty() && tmp[0] != '/')
        tmp = "/" + tmp;

    return prefix + tmp;
}


namespace
{
std::pair<std::string, long> ParseAlphaNumericPin(const std::string& pinNum)
{
    std::string prefix;
    long        numValue = -1;

    size_t numStart = pinNum.size();
    for(int i = static_cast<int>(pinNum.size()) - 1; i >= 0; --i)
    {
        if(!std::isdigit(static_cast<unsigned char>(pinNum[i])))
        {
            numStart = i + 1;
            break;
        }
        if(i == 0)
            numStart = 0; // all digits
    }

    if(numStart < pinNum.size())
    {
        prefix = pinNum.substr(0, numStart);
        std::string numericPart = pinNum.substr(numStart);
        if(!numericPart.empty())
            numValue = std::stol(numericPart);
    }

    return { prefix, numValue };
}
} // namespace

std::vector<std::string> ExpandStackedPinNotation(const std::string& aPinName, bool* aValid)
{
    if(aValid)
        *aValid = true;
    std::vector<std::string> expanded;

    bool hasOpenBracket = aPinName.find('[') != std::string::npos;
    bool hasCloseBracket = aPinName.find(']') != std::string::npos;

    if(hasOpenBracket || hasCloseBracket)
    {
        if(aPinName.empty() || aPinName.front() != '[' || aPinName.back() != ']')
        {
            if(aValid)
                *aValid = false;
            expanded.push_back(aPinName);
            return expanded;
        }
    }

    if(aPinName.front() != '[' || aPinName.back() != ']')
    {
        expanded.push_back(aPinName);
        return expanded;
    }

    std::string inner = aPinName.substr(1, aPinName.size() - 2);
    size_t      start = 0;

    while(start < inner.size())
    {
        size_t      comma = inner.find(',', start);
        std::string part = (comma == std::string::npos) ? inner.substr(start) : inner.substr(start, comma - start);

        // Trim
        part.erase(part.begin(), std::find_if(part.begin(), part.end(),
                                              [](int ch)
                                              {
                                                  return !std::isspace(ch);
                                              }));
        part.erase(std::find_if(part.rbegin(), part.rend(),
                                [](int ch)
                                {
                                    return !std::isspace(ch);
                                })
                           .base(),
                   part.end());

        if(part.empty())
        {
            start = (comma == std::string::npos) ? inner.size() : comma + 1;
            continue;
        }

        size_t dashPos = part.find('-');
        if(dashPos != std::string::npos)
        {
            std::string startTxt = part.substr(0, dashPos);
            std::string endTxt = part.substr(dashPos + 1);

            // Trim
            startTxt.erase(startTxt.begin(), std::find_if(startTxt.begin(), startTxt.end(),
                                                          [](int ch)
                                                          {
                                                              return !std::isspace(ch);
                                                          }));
            startTxt.erase(std::find_if(startTxt.rbegin(), startTxt.rend(),
                                        [](int ch)
                                        {
                                            return !std::isspace(ch);
                                        })
                                   .base(),
                           startTxt.end());

            endTxt.erase(endTxt.begin(), std::find_if(endTxt.begin(), endTxt.end(),
                                                      [](int ch)
                                                      {
                                                          return !std::isspace(ch);
                                                      }));
            endTxt.erase(std::find_if(endTxt.rbegin(), endTxt.rend(),
                                      [](int ch)
                                      {
                                          return !std::isspace(ch);
                                      })
                                 .base(),
                         endTxt.end());

            auto [startPrefix, startVal] = ParseAlphaNumericPin(startTxt);
            auto [endPrefix, endVal] = ParseAlphaNumericPin(endTxt);

            if(startPrefix != endPrefix || startVal == -1 || endVal == -1 || startVal > endVal)
            {
                if(aValid)
                    *aValid = false;
                expanded.clear();
                expanded.push_back(aPinName);
                return expanded;
            }

            for(long ii = startVal; ii <= endVal; ++ii)
            {
                std::ostringstream oss;
                if(startPrefix.empty())
                    oss << ii;
                else
                    oss << startPrefix << ii;
                expanded.push_back(oss.str());
            }
        }
        else
        {
            expanded.push_back(part);
        }

        if(comma == std::string::npos)
            break;
        start = comma + 1;
    }

    if(expanded.empty())
    {
        expanded.push_back(aPinName);
        if(aValid)
            *aValid = false;
    }

    return expanded;
}

int CountStackedPinNotation(const std::string& aPinName, bool* aValid)
{
    size_t len = aPinName.size();

    if(!aValid)
    {
        if(len < 3)
            return 1;
    }
    else
    {
        *aValid = true;
        bool hasOpenBracket = aPinName.find('[') != std::string::npos;
        bool hasCloseBracket = aPinName.find(']') != std::string::npos;

        if(hasOpenBracket || hasCloseBracket)
        {
            if(aPinName.empty() || aPinName.front() != '[' || aPinName.back() != ']')
            {
                *aValid = false;
                return 1;
            }
        }
    }

    if(aPinName.front() != '[' || aPinName.back() != ']')
        return 1;

    std::string inner = aPinName.substr(1, len - 2);
    int         count = 0;
    size_t      start = 0;

    while(start < inner.size())
    {
        size_t      comma = inner.find(',', start);
        std::string part = (comma == std::string::npos) ? inner.substr(start) : inner.substr(start, comma - start);

        // Trim
        part.erase(part.begin(), std::find_if(part.begin(), part.end(),
                                              [](int ch)
                                              {
                                                  return !std::isspace(ch);
                                              }));
        part.erase(std::find_if(part.rbegin(), part.rend(),
                                [](int ch)
                                {
                                    return !std::isspace(ch);
                                })
                           .base(),
                   part.end());

        if(part.empty())
        {
            start = (comma == std::string::npos) ? inner.size() : comma + 1;
            continue;
        }

        size_t dashPos = part.find('-');
        if(dashPos != std::string::npos)
        {
            std::string startTxt = part.substr(0, dashPos);
            std::string endTxt = part.substr(dashPos + 1);

            // Trim
            startTxt.erase(startTxt.begin(), std::find_if(startTxt.begin(), startTxt.end(),
                                                          [](int ch)
                                                          {
                                                              return !std::isspace(ch);
                                                          }));
            startTxt.erase(std::find_if(startTxt.rbegin(), startTxt.rend(),
                                        [](int ch)
                                        {
                                            return !std::isspace(ch);
                                        })
                                   .base(),
                           startTxt.end());

            endTxt.erase(endTxt.begin(), std::find_if(endTxt.begin(), endTxt.end(),
                                                      [](int ch)
                                                      {
                                                          return !std::isspace(ch);
                                                      }));
            endTxt.erase(std::find_if(endTxt.rbegin(), endTxt.rend(),
                                      [](int ch)
                                      {
                                          return !std::isspace(ch);
                                      })
                                 .base(),
                         endTxt.end());

            auto [startPrefix, startVal] = ParseAlphaNumericPin(startTxt);
            auto [endPrefix, endVal] = ParseAlphaNumericPin(endTxt);

            if(startPrefix != endPrefix || startVal == -1 || endVal == -1 || startVal > endVal)
            {
                if(aValid)
                    *aValid = false;
                return 1;
            }

            count += static_cast<int>(endVal - startVal + 1);
        }
        else
        {
            ++count;
        }

        if(comma == std::string::npos)
            break;
        start = comma + 1;
    }

    if(count == 0)
    {
        if(aValid)
            *aValid = false;
        return 1;
    }

    return count;
}

std::string GetDefaultVariantName()
{
    return std::string(defaultVariantName);
}


int SortVariantNames(const std::string& aLhs, const std::string& aRhs)
{
    if((aLhs == defaultVariantName) && (aRhs != defaultVariantName))
        return -1;

    if((aLhs != defaultVariantName) && (aRhs == defaultVariantName))
        return 1;

    return StrNumCmp(aLhs, aRhs);
}

struct LOAD_MESSAGE
{
    std::string message;
    SEVERITY severity;
};


std::vector<LOAD_MESSAGE> ExtractLibraryLoadErrors(const std::string& aErrorString, int aSeverity)
{
    std::vector<LOAD_MESSAGE> messages;

    if(aErrorString.empty())
        return messages;

    std::istringstream iss(aErrorString);
    std::string        line;

    while(std::getline(iss, line))
    {
        // Remove carriage return if present (for Windows-style "\r\n")
        if(!line.empty() && line.back() == '\r')
            line.pop_back();

        // Skip internal code location lines (e.g., "from pcb_io_kicad_sexpr_parser.cpp : ...")
        if(line.size() >= 5 && line.compare(0, 5, "from ") == 0)
            continue;

        // Keep library-level or Expecting lines
        if(line.size() >= 8 && line.compare(0, 8, "Library '") == 0)
            messages.push_back({ line, static_cast<SEVERITY>(aSeverity) });
        else if(line.find("Expecting") != std::string::npos)
            messages.push_back({ line, static_cast<SEVERITY>(aSeverity) });
    }

    return messages;
}