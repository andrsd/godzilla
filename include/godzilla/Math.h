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

template <int N, typename T>
struct pow_impl {
    static inline T
    value(T x)
    {
        if (N % 2)
            return x * pow_impl<N - 1, T>::value(x);
        T x_n_half = pow_impl<N / 2, T>::value(x);
        return x_n_half * x_n_half;
    }
};

template <typename T>
struct pow_impl<1, T> {
    static inline T
    value(T x)
    {
        return x;
    }
};

template <typename T>
struct pow_impl<0, T> {
    static inline T
    value(T)
    {
        return 1;
    }
};

/// Compute `N` power of `x`
///
/// @tparam N Exponent (integer)
/// @param x Operand
/// @return `x` to the power of `e`
template <int N, typename T>
inline T
pow(T x)
{
    return pow_impl<N, T>::value(x);
}

/// Compute `e` power of `x`
///
/// @param x Operand
/// @param e Exponent (integer)
/// @return `x` to the power of `e`
template <typename T>
inline T
pow(T x, int e)
{
    bool neg = false;
    T result = 1.0;

    if (e < 0) {
        neg = true;
        e = -e;
    }

    while (e) {
        // if bit 0 is set multiply the current power of two factor of the exponent
        if (e & 1)
            result *= x;
        // x is incrementally set to consecutive powers of powers of two
        x *= x;
        // bit shift the exponent down
        e >>= 1;
    }

    return neg ? 1.0 / result : result;
}

/// Alias for convenience, or potentially for better implementation
template <typename T, typename EXP>
inline T
pow(T x, EXP exp)
{
    return std::pow(x, exp);
}

} // namespace math
} // namespace godzilla
