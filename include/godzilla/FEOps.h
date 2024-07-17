// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "godzilla/DenseVector.h"
#include "godzilla/DenseMatrix.h"

namespace godzilla {

namespace fe {

/// Compute linear combination of `a` and `b`
///
/// @tparam T Type must be able to be multiplied by a `Real`
/// @tparam N_VALS Number of values
/// @param a Coefficients
/// @param b Elements to be multiplied by coefficients
/// @return Linear combination `a(i) * b(i)`
template <typename T, Int N_VALS>
inline T
linear_combination(const DenseVector<Real, N_VALS> & a, const DenseVector<T, N_VALS> & b)
{
    T res;
    res.set_values(0.);
    for (Int i = 0; i < N_VALS; i++)
        res += a(i) * b(i);
    return res;
}

/// Compute gradient of a scalar quantity
///
/// @tparam D Spatial dimension
/// @tparam N_VALS Number of values per element
/// @param vals Values
/// @param grad_phi Gradient of test functions
/// @return Computed gradient
template <Int D, Int N_VALS>
inline DenseVector<Real, D>
gradient(const DenseVector<Real, N_VALS> & vals, const DenseMatrix<Real, D, N_VALS> & grad_phi)
{
    return grad_phi * vals;
}

/// Compute gradient of a vector-valued quantity
///
/// @tparam C Number of vector component
/// @tparam D Spatial dimension
/// @tparam N Number of values per element
/// @param vals Values
/// @param grad_phi Gradient of test functions
/// @return Computed gradient
template <Int C, Int D, Int N>
inline DenseMatrix<Real, D, C>
gradient(const DenseMatrix<Real, N, C> & vals, const DenseMatrix<Real, D, N> & grad_phi)
{
    return grad_phi * vals;
}

} // namespace fe

} // namespace godzilla
