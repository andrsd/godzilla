// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "godzilla/Assert.h"
#include <algorithm>
#include <cmath>
#include <numbers>
#include <concepts>
#include <type_traits>

namespace godzilla {
namespace math {

/// PI
constexpr Real PI = std::numbers::pi_v<Real>;
/// Square root of 2
constexpr Real SQRT2 = std::numbers::sqrt2_v<Real>;
/// Square root of 3
constexpr Real SQRT3 = std::numbers::sqrt3_v<Real>;
/// Euler's number
constexpr Real E = std::numbers::e_v<Real>;

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

/// Compute the square root of a number.
///
/// @param arg The number to compute the square root of.
template <typename T>
T
sqrt(T arg)
{
    return std::sqrt(arg);
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

/// Compute the factorial
///
/// @param x The number to compute the factorial of.
/// @return The factorial of `x`.
template <typename T>
    requires std::is_integral_v<T>
inline Real
factorial(T x)
{
    expect_true(x >= 0, "Factorial is defined only for positive numbers");
    Real result = 1.0;
    for (Int i = 2; i <= x; ++i)
        result *= i;
    return result;
}

/// Compute sine of an argument
template <typename T>
    requires std::is_floating_point_v<T>
T
sin(T arg)
{
    return std::sin(arg);
}

/// Compute cosine of an argument
template <typename T>
    requires std::is_floating_point_v<T>
T
cos(T arg)
{
    return std::cos(arg);
}

/// Compute tangent of an argument
template <typename T>
    requires std::is_floating_point_v<T>
T
tan(T arg)
{
    return std::tan(arg);
}

/// Compute natural logarithm
template <typename T>
    requires std::is_floating_point_v<T>
T
ln(T arg)
{
    return std::log(arg);
}

/// Compute logarithm with base 2
template <typename T>
    requires std::is_floating_point_v<T>
T
log2(T arg)
{
    return std::log2(arg);
}

/// Compute logarithm with base 10
template <typename T>
    requires std::is_floating_point_v<T>
T
log10(T arg)
{
    return std::log10(arg);
}

/// Compute logarithm with any base
template <typename T>
    requires std::is_floating_point_v<T>
T
log(T base, T arg)
{
    return std::log(arg) / std::log(base);
}

} // namespace math
} // namespace godzilla
