#pragma once

#include <string>

namespace godzilla {
namespace utils {

bool pathExists(const std::string & path);

/**
 * Convert supplied string to upper case.
 * @param name The string to convert upper case.
 */
std::string toUpper(const std::string & name);

/**
 * Convert supplied string to lower case.
 * @param name The string to convert upper case.
 */
std::string toLower(const std::string & name);

} // namespace utils
} // namespace godzilla
