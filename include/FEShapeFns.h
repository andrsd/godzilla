#pragma once

#include "CallStack.h"
#include "Types.h"
#include "Array1D.h"
#include "Array2D.h"
#include "DenseVector.h"

namespace godzilla {

namespace fe {

/// Compute gradients of shape functions
///
/// @tparam ELEM_TYPE Element type
/// @tparam DIM Spatial dimension
/// @tparam N_ELEM_NODES Number of nodes
/// @param coords Element coordinates
/// @param volume Element volume
/// @return Vector of gradients of shape functions associated with element nodes
template <ElementType ELEM_TYPE, int DIM, Int N_ELEM_NODES = get_num_element_nodes(ELEM_TYPE)>
inline DenseVector<DenseVector<Real, DIM>, N_ELEM_NODES>
grad_shape(const DenseVector<DenseVector<Real, DIM>, N_ELEM_NODES> & coords, Real volume)
{
    GODZILLA_UNUSED(coords);
    GODZILLA_UNUSED(volume);

    error("Calculation of shape function gradients is not implemented for element '{}' yet.",
          get_element_type_str(ELEM_TYPE));
}

/// Compute gradients of shape functions of EDGE2 in 1-D
template <>
inline DenseVector<DenseVector<Real, 1>, 2>
grad_shape<EDGE2, 1>(const DenseVector<DenseVector<Real, 1>, 2> & coords, Real volume)
{
    GODZILLA_UNUSED(coords);

    _F_;
    Real c = 1 / volume;
    DenseVector<DenseVector<Real, 1>, 2> grads;
    grads(0) = DenseVector<Real, 1>({ -c });
    grads(1) = DenseVector<Real, 1>({ c });
    return grads;
}

/// Compute gradients of shape functions of TRI3 in 2-D
template <>
inline DenseVector<DenseVector<Real, 2>, 3>
grad_shape<TRI3, 2>(const DenseVector<DenseVector<Real, 2>, 3> & coords, Real volume)
{
    _F_;
    auto n0 = coords(0);
    auto n1 = coords(1);
    auto n2 = coords(2);

    auto x1 = n0(0);
    auto x2 = n1(0);
    auto x3 = n2(0);
    auto y1 = n0(1);
    auto y2 = n1(1);
    auto y3 = n2(1);

    Real x31 = x3 - x1;
    Real x21 = x2 - x1;
    Real y31 = y3 - y1;
    Real y21 = y2 - y1;

    Real c = 0.5 / volume;
    DenseVector<DenseVector<Real, 2>, 3> grads;
    grads.set(0) = DenseVector<Real, 2>({ c * (y21 - y31), c * (x31 - x21) });
    grads.set(1) = DenseVector<Real, 2>({ c * y31, -c * x31 });
    grads.set(2) = DenseVector<Real, 2>({ -c * y21, c * x21 });
    return grads;
}

//

template <ElementType ELEM_TYPE, Int DIM, Int N_ELEM_NODES = get_num_element_nodes(ELEM_TYPE)>
inline Array1D<DenseVector<DenseVector<Real, DIM>, N_ELEM_NODES>>
calc_grad_shape(const Array1D<DenseVector<Real, DIM>> & coords,
                const Array1D<DenseVector<Int, N_ELEM_NODES>> & connect,
                const Array1D<Real> & volumes)
{
    _F_;
    Array1D<DenseVector<DenseVector<Real, DIM>, N_ELEM_NODES>> grad_shfns(connect.get_size());
    for (godzilla::Int ie = 0; ie < connect.get_size(); ie++) {
        auto idx = connect.get(ie);
        auto elem_coord = coords.get_values(idx);
        auto volume = volumes(ie);
        grad_shfns(ie) = grad_shape<ELEM_TYPE, DIM>(elem_coord, volume);
    }
    return grad_shfns;
}

} // namespace fe

} // namespace godzilla
