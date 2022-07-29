#pragma once

#include <string>
#include <typeinfo>

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

} // namespace utils
} // namespace godzilla
