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
gradient(const DenseVector<Real, N_VALS> & vals, const DenseMatrix<Real, N_VALS, D> & grad_phi)
{
    return vals * grad_phi;
}

/// Compute gradient of a vector-valued quantity
///
/// @tparam N_COMPS Number of vector component
/// @tparam D Spatial dimension
/// @tparam N_VALS Number of values per element
/// @param vals Values
/// @param grad_phi Gradient of test functions
/// @return Computed gradient
template <Int N_COMPS, Int D, Int N_VALS>
inline DenseMatrix<Real, D, N_COMPS>
gradient(const DenseVector<DenseVector<Real, N_COMPS>, N_VALS> & vals,
         const DenseMatrix<Real, N_VALS, D> & grad_phi)
{
    DenseMatrix<Real, D, N_COMPS> grad;
    grad.set_values(0.);
    for (Int i = 0; i < D; i++) {
        for (Int j = 0; j < N_COMPS; j++)
            for (Int k = 0; k < N_VALS; k++)
                grad(i, j) += vals(k)(j) * grad_phi(k, i);
    }
    return grad;
}

/// Compute gradient of a vector-valued quantity
///
/// @tparam N_COMPS Number of vector component
/// @tparam D Spatial dimension
/// @tparam N_VALS Number of values per element
/// @param vals Values
/// @param grad_phi Gradient of test functions
/// @return Computed gradient
template <Int N_COMPS, Int D, Int N_VALS>
inline DenseMatrix<Real, D, N_COMPS>
gradient(const DenseMatrix<Real, N_VALS, N_COMPS> & vals,
         const DenseMatrix<Real, N_VALS, D> & grad_phi)
{
    DenseMatrix<Real, D, N_COMPS> grad;
    grad.set_values(0.);
    for (Int i = 0; i < D; i++) {
        for (Int j = 0; j < N_COMPS; j++)
            for (Int k = 0; k < N_VALS; k++)
                grad(i, j) += vals(k, j) * grad_phi(k, i);
    }
    return grad;
}

} // namespace fe

} // namespace godzilla
