#ifndef MARKUP_PARSER_H
#define MARKUP_PARSER_H

#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/parse_tree.hpp>
#include <iostream>
#include <string>
#include <utf8.hxx>


namespace MARKUP
{
using namespace tao::pegtl;

struct subscript;
struct superscript;
struct overbar;
struct url;

struct NODE : parse_tree::basic_node<NODE>
{
    std::string asString() const;

    std::string typeString() const;

    std::string asWxString() const;

    bool isOverbar() const     { return is_type<MARKUP::overbar>(); }
    bool isSubscript() const   { return is_type<MARKUP::subscript>(); }
    bool isSuperscript() const { return is_type<MARKUP::superscript>(); }
    bool isURL() const         { return is_type<MARKUP::url>(); }
};

struct markup : sor< subscript,
                     superscript,
                     overbar > {};

/**
 * anyString =
 * a run of characters that do not start a command sequence, or if they do, they do not start
 * a complete command prefix (command char + open brace)
 */
struct anyString : plus< seq< not_at< markup >,
                              not_at< url >,
                              utf8::any > > {};

struct escapeSequence : seq< string<'{'>, identifier, string<'}'> > {};

struct anyStringWithinBraces : plus< sor< seq< not_at< markup >,
                                               escapeSequence >,
                                          seq< not_at< markup >,
                                               utf8::not_one<'}'> > > > {};

struct url : seq< sor< string<'h', 't', 't', 'p', ':', '/', '/'>,
                       string<'h', 't', 't', 'p', 's', ':', '/', '/'> >,
                  plus< utf8::not_one<' '> > > {};

template< typename ControlChar >
struct braces : seq< seq< ControlChar,
                          string<'{'> >,
                     until< string<'}'>,
                            sor< anyStringWithinBraces,
                                 subscript,
                                 superscript,
                                 overbar > > > {};

struct superscript : braces< string<'^'> > {};
struct subscript   : braces< string<'_'> > {};
struct overbar     : braces< string<'~'> > {};

/**
 * Finally, the full grammar
 */
struct anything : sor< anyString,
                       url,
                       subscript,
                       superscript,
                       overbar > {};

struct grammar : until< tao::pegtl::eof, anything > {};

template <typename Rule>
using selector = parse_tree::selector< Rule,
                                       parse_tree::store_content::on< anyStringWithinBraces,
                                                                      url,
                                                                      anyString >,
                                       parse_tree::discard_empty::on< superscript,
                                                                      subscript,
                                                                      overbar > >;

class MARKUP_PARSER
{
public:
    MARKUP_PARSER( const std::string& source ) :
            in( std::make_unique<string_input<>>( source, "from_input" ) ),
            mem_in()
    {}

    MARKUP_PARSER( const std::string* source ) :
            in(),
            mem_in( std::make_unique<memory_input<>>( *source, "from_input" ) )
    {}

    std::unique_ptr<NODE> Parse();

private:
    std::unique_ptr<string_input<>> in;
    std::unique_ptr<memory_input<>> mem_in;
};

} // namespace MARKUP


#endif //MARKUP_PARSER_H
