#ifndef MAP_HELPERS_H_
#define MAP_HELPERS_H_

#include <optional>
#include <map>
#include <string>


template <typename V>
inline std::optional<V> get_opt( const std::map<std::string, V>& aMap, const std::string& aKey )
{
    auto it = aMap.find( aKey );

    if( it == aMap.end() )
        return std::nullopt;

    return it->second;
}


template <typename V>
inline std::optional<V> get_opt( const std::map<std::string, V>& aMap, const char* aKey )
{
    return get_opt( aMap, std::string( aKey ) );
}


template <typename K, typename V>
inline std::optional<V> get_opt( const std::map<K, V>& aMap, const K& aKey )
{
    auto it = aMap.find( aKey );

    if( it == aMap.end() )
        return std::nullopt;

    return it->second;
}


inline std::string get_def( const std::map<std::string, std::string>& aMap, const char* aKey,
                         const char* aDefval = "" )
{
    typename std::map<std::string, std::string>::const_iterator it =
            aMap.find( std::string( aKey ) );
    if( it == aMap.end() )
    {
        return std::string( aDefval );
    }
    else
    {
        return it->second;
    }
}


inline std::string get_def( const std::map<std::string, std::string>& aMap, const char* aKey,
                         const std::string& aDefval = std::string() )
{
    typename std::map<std::string, std::string>::const_iterator it =
            aMap.find( std::string( aKey ) );
    if( it == aMap.end() )
    {
        return aDefval;
    }
    else
    {
        return it->second;
    }
}


inline std::string get_def( const std::map<std::string, std::string>& aMap, const std::string& aKey,
                         const std::string& aDefval = std::string() )
{
    typename std::map<std::string, std::string>::const_iterator it = aMap.find( aKey );
    if( it == aMap.end() )
    {
        return aDefval;
    }
    else
    {
        return it->second;
    }
}


template <typename K, typename V>
inline V get_def( const std::map<K, V>& aMap, const K& aKey, const V& aDefval = V() )
{
    typename std::map<K, V>::const_iterator it = aMap.find( aKey );
    if( it == aMap.end() )
    {
        return aDefval;
    }
    else
    {
        return it->second;
    }
}


#endif // MAP_HELPERS_H_
