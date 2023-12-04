// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

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

} // namespace math
} // namespace godzilla
