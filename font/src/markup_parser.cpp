#include <markup_parser.hxx>
#include <sstream>
#include <string_utils.hxx>

using namespace MARKUP;


std::unique_ptr<NODE> MARKUP_PARSER::Parse()
{
    try
    {
        std::unique_ptr<NODE> root;

        if( mem_in )
            root = parse_tree::parse<MARKUP::grammar, MARKUP::NODE, MARKUP::selector>( *mem_in );
        else
            root = parse_tree::parse<MARKUP::grammar, MARKUP::NODE, MARKUP::selector>( *in );

        return root;
    }
    catch ( tao::pegtl::parse_error& )
    {
        // couldn't parse text item
        // TODO message to user?
        return nullptr;
    }
}


std::string NODE::typeString() const
{
    std::stringstream os;

    if( is_type<MARKUP::subscript>() )                  os << "SUBSCRIPT";
    else if( is_type<MARKUP::superscript>() )           os << "SUPERSCRIPT";
    else if( is_type<MARKUP::overbar>() )               os << "OVERBAR";
    else if( is_type<MARKUP::anyString>() )             os << "ANYSTRING";
    else if( is_type<MARKUP::url>() )                   os << "URL";
    else if( is_type<MARKUP::anyStringWithinBraces>() ) os << "ANYSTRINGWITHINBRACES";
    else                                                os << "other";

    return os.str();
}


std::string NODE::asWxString() const
{
    return From_UTF8( string().c_str() );
}


std::string NODE::asString() const
{
    std::stringstream os;

    os << type;

    if( has_content() )
        os << " \"" << string() << "\"";

    return os.str();
}
