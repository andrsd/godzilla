// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <vector>
#include <map>
#include <string>
#include <typeinfo>
#include <utility>
#include <petsclog.h>
#include "fmt/printf.h"

namespace godzilla {
namespace utils {

bool path_exists(const std::string & path);

/**
 * Convert supplied string to upper case.
 * @param name The string to convert upper case.
 */
std::string to_upper(const std::string & name);

/**
 * Convert supplied string to lower case.
 * @param name The string to convert upper case.
 */
std::string to_lower(const std::string & name);

/**
 * Check if string `str` ends with `suffix`
 *
 * @param str String to check
 * @param suffix The expected suffix
 */
bool has_suffix(const std::string & str, const std::string & suffix);

/**
 * Check if string `str` ends with specified text
 *
 * @param str String to check
 * @param end The expected text
 */
bool ends_with(const std::string & str, const std::string & end);

/**
 * Check if string `str` starts with `prefix`
 *
 * @param str String to check
 * @param prefix The expected prefix
 */
bool has_prefix(const std::string & str, const std::string & prefix);

/**
 * Check if string `str` starts with specified text
 *
 * @param str String to check
 * @param start The expected text
 */
bool starts_with(const std::string & str, const std::string & start);

template <typename T>
std::string
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
std::string human_time(PetscLogDouble time);

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
    for (std::size_t i = 0; i < array.size(); i++)
        if (array[i] == value)
            return i;
    throw std::runtime_error(fmt::format("Did not find {} in array", value));
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

} // namespace godzilla
