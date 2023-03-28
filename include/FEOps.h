#pragma once

#include "Types.h"
#include "DenseVector.h"

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
T
linear_combination(const DenseVector<Real, N_VALS> & a, const DenseVector<T, N_VALS> & b)
{
    T res;
    res.set_values(0.);
    for (Int i = 0; i < N_VALS; i++)
        res += a(i) * b(i);
    return res;
}

/// Compute gradient(u) * gradient(phi) (at nodes)
///
/// @tparam DIM Spatial dimension
/// @tparam N_VALS Number of values (at nodes)
/// @param grad_u Gradient of variable u
/// @param grad_phi Gradients of shape functions (at nodes)
/// @return Vector of grad(u)*grad(phi)_i (at nodes)
template <Int DIM, Int N_VALS>
DenseVector<Real, N_VALS>
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
