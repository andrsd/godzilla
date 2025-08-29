// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include <algorithm>
#include <cmath>

namespace godzilla {
namespace math {

/// Sign of the argument
///
/// @tparam T Type
/// @param val Value to get the sign of
/// @return Sign of the value, i.e. -1 for negative values, 0 for 0, and 1 for positive values
template <typename T>
int
sign(T val)
{
    return (T(0) < val) - (val < T(0));
}

/// Square of the argument
///
/// @tparam T Type
/// @param val Value to be squared
/// @return Square of the value, i.e. `val * val`
template <typename T>
T
sqr(T val)
{
    return val * val;
}

/// Maximum
template <typename T>
const T &
max(const T & a, const T & b)
{
    return std::max(a, b);
}

template <class T>
T
max(std::initializer_list<T> ilist)
{
    return std::max(ilist);
}

/// Minimum
template <typename T>
const T &
min(const T & a, const T & b)
{
    return std::min(a, b);
}

template <class T>
T
min(std::initializer_list<T> ilist)
{
    return std::min(ilist);
}

/// Absolute value
inline Real
abs(Real val)
{
    return std::abs(val);
}

} // namespace math
} // namespace godzilla
