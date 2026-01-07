// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Assert.h"
#include <sys/types.h>
#include <initializer_list>
#include <vector>
#include <type_traits>

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
in<String>(String value, const std::vector<String> & options)
{
    String v = value.to_lower();
    return std::any_of(options.cbegin(), options.cend(), [v](String o) {
        return v == o.to_lower();
    });
}

template <>
inline bool
in<const char *>(const char * value, const std::vector<const char *> & options)
{
    String v = String(value).to_lower();
    return std::any_of(options.cbegin(), options.cend(), [v](String o) {
        return v == o.to_lower();
    });
}

/// Check that `value` is within range
///
/// @param value Value to test
/// @param range Range to test against
/// @return `true` if `value` is within range, `false` otherwise
template <typename T, typename U>
inline bool
in_range(T value, std::initializer_list<U> range)
{
    static_assert(std::is_convertible<T, U>::value, "T must be convertible to U");
    expect_true(range.size() == 2, "Range must have exactly two elements.");
    auto it = range.begin();
    U lo = *it;
    U hi = *(++it);
    return value >= lo && value <= hi;
}

} // namespace validation
} // namespace godzilla
