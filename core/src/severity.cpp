#include <severity.hxx>
#include <vector>


std::string formatSeverities( int aSeverities )
{
    std::string result;
    std::vector<std::string> items;

    if( aSeverities & RPT_SEVERITY_ERROR )
        items.push_back( "Errors" );

    if( aSeverities & RPT_SEVERITY_WARNING )
        items.push_back( "Warnings" );

    if( aSeverities & RPT_SEVERITY_EXCLUSION )
        items.push_back( "Exclusions" );

    if( items.empty() )
        return "None";

    for( size_t i = 0; i < items.size(); i++ )
    {
        result += items[i];

        if( i < items.size() - 1 )
            result += ", ";
    }

    return result;
}

