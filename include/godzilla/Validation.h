// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Utils.h"
#include <vector>
#include <string>

namespace godzilla {
namespace validation {

/// Check that `value` is equal to one of the `options`
///
/// @return `true` if `value` is one of the `options`, `false` otherwise
/// @param value Value to test
/// @param options Possible options
template <typename T>
inline bool
in(T value, const std::vector<T> & options)
{
    return std::any_of(options.cbegin(), options.cend(), [value](const T & o) {
        return value == o;
    });
}

template <>
inline bool
in<std::string>(std::string value, const std::vector<std::string> & options)
{
    std::string v = utils::to_lower(value);
    return std::any_of(options.cbegin(), options.cend(), [v](const std::string & o) {
        return v == utils::to_lower(o);
    });
}

template <>
inline bool
in<const char *>(const char * value, const std::vector<const char *> & options)
{
    std::string v = utils::to_lower(value);
    return std::any_of(options.cbegin(), options.cend(), [v](const std::string & o) {
        return v == utils::to_lower(o);
    });
}

} // namespace validation
} // namespace godzilla
