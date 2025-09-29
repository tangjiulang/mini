#ifndef UTIL_H
#define UTIL_H

#include <cassert>
#include <cmath>
#include <cstdint>
#include <limits>
#include <typeinfo>
#include <type_traits>
#include <algorithm>

 /**
  * Helper to avoid directly including wx/log.h for the templated functions in kimath
  */
void mathLogDebug(const char* aFormatString, ...);

/**
 * Workaround to avoid the empty-string conversion issue in wxWidgets
 */
void mathLogOverflow(double v, const char* aTypeName);


// Suppress an annoying warning that the explicit rounding we do is not precise
#ifdef HAVE_WIMPLICIT_FLOAT_CONVERSION
_Pragma("GCC diagnostic push") \
_Pragma("GCC diagnostic ignored \"-Wimplicit-int-float-conversion\"")
#endif


/**
 * Perform a cast between numerical types. Will clamp the return value to numerical type limits.
 *
 * In Debug build an assert fires if will not fit into the return type.
 */
    template <typename in_type = long long int, typename ret_type = int>
inline constexpr ret_type KiCheckedCast(in_type v)
{
    if constexpr (std::is_same_v<in_type, long long int> && std::is_same_v<ret_type, int>)
    {
        if (v > std::numeric_limits<int>::max())
        {
            mathLogOverflow(double(v), typeid(int).name());

            return std::numeric_limits<int>::max();
        }
        else if (v < std::numeric_limits<int>::lowest())
        {
            mathLogOverflow(double(v), typeid(int).name());

            return std::numeric_limits<int>::lowest();
        }

        return int(v);
    }
    else
    {
        return v;
    }
}


/**
 * Round a numeric value to an integer using "round halfway cases away from zero" and
 * clamp the result to the limits of the return type.
 *
 * In Debug build an assert fires if will not fit into the return type.
 */
template <typename fp_type, typename ret_type = int>
constexpr ret_type KiROUND(fp_type v, bool aQuiet = false)
{
    using limits = std::numeric_limits<ret_type>;

#if __cplusplus >= 202302L // isnan is not constexpr until C++23
    if constexpr (std::is_floating_point_v<fp_type>)
    {
        if (std::isnan(v))
        {
            if (!aQuiet)
                mathLogOverflow(double(v), typeid(ret_type).name());

            return 0;
        }
    }
#endif

    long long rounded = std::llround(v);
    long long clamped = std::clamp<long long>(rounded,
        static_cast<long long>(limits::lowest()),
        static_cast<long long>(limits::max()));

    if (!aQuiet && clamped != rounded)
        mathLogOverflow(double(v), typeid(ret_type).name());

    return static_cast<ret_type>(clamped);
}

#ifdef HAVE_WIMPLICIT_FLOAT_CONVERSION
_Pragma("GCC diagnostic pop")
#endif

/**
 * Scale a number (value) by rational (numerator/denominator). Numerator must be <= denominator.
 */

    template <typename T>
T rescale(T aNumerator, T aValue, T aDenominator)
{
    return aNumerator * aValue / aDenominator;
}

template <typename T>
constexpr int sign(T val)
{
    return (T(0) < val) - (val < T(0));
}

// explicit specializations for integer types, taking care of overflow.
template <>
int rescale(int aNumerator, int aValue, int aDenominator);

template <>
int64_t rescale(int64_t aNumerator, int64_t aValue, int64_t aDenominator);


/**
 * Template to compare two floating point values for equality within a required epsilon.
 *
 * @param aFirst value to compare.
 * @param aSecond value to compare.
 * @param aEpsilon allowed error.
 * @return true if the values considered equal within the specified epsilon, otherwise false.
 */
template <class T>
typename std::enable_if<std::is_floating_point<T>::value, bool>::type
equals(T aFirst, T aSecond, T aEpsilon = std::numeric_limits<T>::epsilon())
{
    const T diff = std::abs(aFirst - aSecond);

    if (diff < aEpsilon)
    {
        return true;
    }

    aFirst = std::abs(aFirst);
    aSecond = std::abs(aSecond);
    T largest = aFirst > aSecond ? aFirst : aSecond;

    if (diff <= largest * aEpsilon)
    {
        return true;
    }

    return false;
}


#endif // UTIL_H
