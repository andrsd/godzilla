// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "godzilla/String.h"
#include <vector>
#include <map>
#include <typeinfo>
#include <utility>
#include <sstream>
#include <petsclog.h>
#include "fmt/printf.h"

namespace godzilla {

class PrintInterface;
class UnstructuredMesh;

namespace utils {

template <typename T>
String
type_name()
{
    return typeid(T).name();
}

/**
 *
 */
template <typename T, typename U>
std::vector<T>
map_keys(const std::map<T, U> & m)
{
    std::vector<T> k;
    k.reserve(m.size());
    for (auto && it : m)
        k.push_back(it.first);
    return k;
}

template <typename T, typename U>
std::vector<T>
map_values(const std::map<T, U> & m)
{
    std::vector<T> v;
    v.reserve(m.size());
    for (auto && it : m)
        v.push_back(it.second);
    return v;
}

/// Provide human readable time
///
/// @param time Time in seconds
/// @return Formatted string with human readable time
String human_time(PetscLogDouble time);

/// Provide human readable number
///
/// @param number Number to format
/// @return Formatted string with human readable number
template <typename T>
String
human_number(T number)
{
    auto num_str = std::to_string(number);
    int insert_position = num_str.length() - 3;
    while (insert_position > 0) {
        num_str.insert(insert_position, ",");
        insert_position -= 3;
    }
    return num_str;
}

/// Convert C++ names into human readable names
String human_type_name(String type);

/// Get index of an value in a std::vector
///
/// @tparam T Type
/// @param array Array of values
/// @param value The value we are looking for
/// @return Index in the array
///
/// NOTE:
/// 1. This function will find only the first value, so make sure that `array` does not have
///    duplicates unless you are really looking for just the first one.
template <typename T>
std::size_t
index_of(const std::vector<T> & array, T value)
{
    for (std::size_t i = 0; i < array.size(); ++i)
        if (array[i] == value)
            return i;
    throw std::runtime_error(fmt::format("Did not find {} in array", value));
}

/// Demangle type name
///
/// @param mangled_name Mangled name
/// @return Demangled name
String demangle(String mangled_name);

/// Safely compute ratio of 2 numbers (avoiding division by zero)
///
/// @param num Numerator
/// @param den Denominator
/// @return Computed ratio
template <typename T>
T
ratio(T num, T den)
{
    if (den != 0.)
        return num / den;
    else
        return 0.;
}

/// Mark for unreachable code
///
/// This is defined as `std::unreachable` in C++23, so we need this ATM.
/// @note Taken from https://en.cppreference.com/w/cpp/utility/unreachable.html
[[noreturn]] inline void
unreachable()
{
    // Uses compiler specific extensions if possible.
    // Even if no extension is used, undefined behavior is still raised by
    // an empty function body and the noreturn attribute.
#if defined(_MSC_VER) && !defined(__clang__) // MSVC
    __assume(false);
#else // GCC, Clang
    __builtin_unreachable();
#endif
}

} // namespace utils

/// Enumerate for iterable containters
///
/// Use like this:
/// ```
/// std::vector<int> v = { 10, 11, 12, 14, 15 };
/// for (const auto & [i, value] : godzilla::enumerate(v))
///     std::cout << i << " " << value << std::endl;
/// ```
template <typename ITERABLE>
auto
enumerate(ITERABLE && iterable)
{
    using std::begin;
    using std::end;
    using Iterator = decltype(begin(iterable));

    struct Enumerator {
        Iterator iter;
        size_t index;

        bool
        operator!=(const Enumerator & other) const
        {
            return iter != other.iter;
        }

        void
        operator++()
        {
            ++iter;
            ++index;
        }

        auto
        operator*() const
        {
            return std::make_pair(index, *iter);
        }
    };

    struct IterableWrapper {
        ITERABLE & iterable;

        auto
        begin()
        {
            return Enumerator { std::begin(iterable), 0 };
        }

        auto
        end()
        {
            return Enumerator { std::end(iterable), 0 };
        }
    };

    return IterableWrapper { iterable };
}

void print_converged_reason(PrintInterface & pi, bool converged);

/// Get block ID from region name
///
/// If block ID is a number, then it is returned as a `Int` type. If it is anything else, we assume
/// it is a cell set name and we try to convert it into cell set ID.
///
/// @param mesh Unstructure mesh we operate on for cell set lookups
/// @param region Region name/Block ID
/// @return Block ID corresponding to the region
Int get_block_id_from_region(const UnstructuredMesh & mesh, String region);

template <typename T>
class Array1D;

/// Join array of values
///
/// @param con String to connect values with
/// @param array Values to connect
/// @return String with connected values
template <typename T>
String
join(const char * con, const Array1D<T> & array)
{
    std::ostringstream oss;
    for (size_t i = 0; i < array.size(); ++i) {
        if (i > 0)
            oss << con;
        oss << array(i);
    }
    return oss.str();
}

/// Join values in std::vector
///
/// @param con String to connect values with
/// @param array Values to connect
/// @return String with connected values
template <typename T>
String
join(const char * con, const std::vector<T> & array)
{
    std::ostringstream oss;
    for (size_t i = 0; i < array.size(); ++i) {
        if (i > 0)
            oss << con;
        oss << array[i];
    }
    return oss.str();
}

/// Split string into parts spearated by delimiter
///
/// @param delim Delimiter
/// @param line LIOne to split
/// @return Individual parts
std::vector<String> split(const char * delim, String line);

} // namespace godzilla
