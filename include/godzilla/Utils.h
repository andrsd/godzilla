#pragma once

#include <vector>
#include <map>
#include <string>
#include <typeinfo>
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
 * Check if string `str` starts with `prefix`
 *
 * @param str String to check
 * @param prefix The expected prefix
 */
bool has_prefix(const std::string & str, const std::string & prefix);

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
} // namespace godzilla
