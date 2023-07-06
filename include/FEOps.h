#pragma once

#include "Types.h"
#include "DenseVector.h"
#include "DenseMatrix.h"

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
/// @param vals Nodal values
/// @param grad_phi Gradient of test functions
/// @return Computed gradient
template <Int D, Int N_VALS>
inline DenseVector<Real, D>
gradient(const DenseVector<Real, N_VALS> & vals,
         const DenseVector<DenseVector<Real, D>, N_VALS> & grad_phi)
{
    return fe::linear_combination(vals, grad_phi);
}

/// Compute gradient of a vector-valued quantity
///
/// @tparam N_COMPS Number of vector component
/// @tparam D Spatial dimension
/// @tparam N_VALS Number of values per element
/// @param vals Nodal values
/// @param grad_phi Gradient of test functions
/// @return Computed gradient
template <Int N_COMPS, Int D, Int N_VALS>
inline DenseMatrix<Real, N_COMPS, D>
gradient(const DenseVector<DenseVector<Real, N_COMPS>, N_VALS> & vals,
         const DenseVector<DenseVector<Real, D>, N_VALS> & grad_phi)
{
    DenseMatrix<Real, N_COMPS, D> grad;
    grad.set_values(0.);
    for (Int i = 0; i < N_COMPS; i++) {
        for (Int j = 0; j < N_VALS; j++)
            for (Int k = 0; k < D; k++)
                grad(i, k) += vals(j)(i) * grad_phi(j)(k);
    }
    return grad;
}

/// Compute gradient(u) * gradient(phi) (at nodes)
///
/// @tparam DIM Spatial dimension
/// @tparam N_VALS Number of values (at nodes)
/// @param grad_u Gradient of variable u
/// @param grad_phi Gradients of shape functions (at nodes)
/// @return Vector of grad(u)*grad(phi)_i (at nodes)
template <Int DIM, Int N_VALS>
inline DenseVector<Real, N_VALS>
grad_u_grad_phi(const DenseVector<Real, DIM> & grad_u,
                const DenseVector<DenseVector<Real, DIM>, N_VALS> & grad_phi)
{
    DenseVector<Real, N_VALS> res;
    for (Int i = 0; i < N_VALS; i++)
        res(i) = grad_u * grad_phi(i);
    return res;
}

} // namespace fe

} // namespace godzilla
