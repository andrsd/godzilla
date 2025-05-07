// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/CallStack.h"
#include "godzilla/Types.h"
#include "godzilla/Convert.h"
#include "godzilla/Array1D.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/DenseVector.h"
#include "godzilla/DenseMatrix.h"
#include "petscdmplex.h"

namespace godzilla {

namespace fe {

/// Compute gradients of shape functions
///
/// @tparam ELEM_TYPE Element type
/// @tparam D Spatial dimension
/// @tparam N Number of nodes
/// @param coords Element coordinates
/// @param volume Element volume
/// @return Vector of gradients of shape functions associated with element nodes
template <ElementType ELEM_TYPE, int D, Int N = get_num_element_nodes(ELEM_TYPE)>
inline DenseMatrix<Real, D, N>
grad_shape(const DenseMatrix<Real, N, D> & coords, Real volume)
{
    throw NotImplementedException(
        "Calculation of shape function gradients is not implemented for element '{}' yet",
        conv::to_str(ELEM_TYPE));
}

/// Compute gradients of shape functions of EDGE2 in 1-D
template <>
inline DenseMatrix<Real, 1, 2>
grad_shape<EDGE2, 1>(const DenseMatrix<Real, 2, 1> & coords, Real volume)
{
    CALL_STACK_MSG();
    Real c = 1 / volume;
    DenseMatrix<Real, 1, 2> grads;
    grads(0, 0) = -c;
    grads(0, 1) = c;
    return grads;
}

/// Compute gradients of shape functions of TRI3 in 2-D
template <>
inline DenseMatrix<Real, 2, 3>
grad_shape<TRI3, 2>(const DenseMatrix<Real, 3, 2> & coords, Real volume)
{
    CALL_STACK_MSG();
    auto x1 = coords(0, 0);
    auto x2 = coords(1, 0);
    auto x3 = coords(2, 0);
    auto y1 = coords(0, 1);
    auto y2 = coords(1, 1);
    auto y3 = coords(2, 1);

    DenseMatrix<Real, 2, 3> grads;
    grads.set_col(0, { (y2 - y3), (x3 - x2) });
    grads.set_col(1, { (y3 - y1), (x1 - x3) });
    grads.set_col(2, { (y1 - y2), (x2 - x1) });
    grads.scale(0.5 / volume);
    return grads;
}

/// Compute gradients of shape functions of TET4 in 3-D
template <>
inline DenseMatrix<Real, 3, 4>
grad_shape<TET4, 3>(const DenseMatrix<Real, 4, 3> & coords, Real volume)
{
    // See [1] for details
    // [1] https://en.wikipedia.org/wiki/Barycentric_coordinate_system
    CALL_STACK_MSG();
    auto x1 = coords(0, 0);
    auto y1 = coords(0, 1);
    auto z1 = coords(0, 2);

    auto x2 = coords(1, 0);
    auto y2 = coords(1, 1);
    auto z2 = coords(1, 2);

    auto x3 = coords(2, 0);
    auto y3 = coords(2, 1);
    auto z3 = coords(2, 2);

    auto x4 = coords(3, 0);
    auto y4 = coords(3, 1);
    auto z4 = coords(3, 2);

    DenseMatrix<Real, 3, 4> grads;
    auto ddx1 = -y2 * z3 + y2 * z4 + y3 * z2 - y3 * z4 - y4 * z2 + y4 * z3;
    auto ddx2 = y1 * z3 - y1 * z4 - y3 * z1 + y3 * z4 + y4 * z1 - y4 * z3;
    auto ddx3 = -y1 * z2 + y1 * z4 + y2 * z1 - y2 * z4 - y4 * z1 + y4 * z2;
    auto ddx4 = y1 * z2 - y1 * z3 - y2 * z1 + y2 * z3 + y3 * z1 - y3 * z2;

    auto ddy1 = x2 * z3 - x2 * z4 - x3 * z2 + x3 * z4 + x4 * z2 - x4 * z3;
    auto ddy2 = -x1 * z3 + x1 * z4 + x3 * z1 - x3 * z4 - x4 * z1 + x4 * z3;
    auto ddy3 = x1 * z2 - x1 * z4 - x2 * z1 + x2 * z4 + x4 * z1 - x4 * z2;
    auto ddy4 = -x1 * z2 + x1 * z3 + x2 * z1 - x2 * z3 - x3 * z1 + x3 * z2;

    auto ddz1 = -x2 * y3 + x2 * y4 + x3 * y2 - x3 * y4 - x4 * y2 + x4 * y3;
    auto ddz2 = x1 * y3 - x1 * y4 - x3 * y1 + x3 * y4 + x4 * y1 - x4 * y3;
    auto ddz3 = -x1 * y2 + x1 * y4 + x2 * y1 - x2 * y4 - x4 * y1 + x4 * y2;
    auto ddz4 = x1 * y2 - x1 * y3 - x2 * y1 + x2 * y3 + x3 * y1 - x3 * y2;

    grads.set_col(0, { ddx1, ddy1, ddz1 });
    grads.set_col(1, { ddx2, ddy2, ddz2 });
    grads.set_col(2, { ddx3, ddy3, ddz3 });
    grads.set_col(3, { ddx4, ddy4, ddz4 });
    grads.scale(1. / 6. / volume);
    return grads;
}

template <ElementType ELEM_TYPE, Int DIM, Int N_ELEM_NODES = get_num_element_nodes(ELEM_TYPE)>
inline Array1D<DenseMatrix<Real, DIM, N_ELEM_NODES>>
calc_grad_shape(const Array1D<DenseVector<Real, DIM>> & coords,
                const Array1D<DenseVector<Int, N_ELEM_NODES>> & connect,
                const Array1D<Real> & volumes)
{
    CALL_STACK_MSG();
    Array1D<DenseMatrix<Real, DIM, N_ELEM_NODES>> grad_shfns(connect.size());
    for (Int ie = 0; ie < connect.size(); ++ie) {
        auto idx = connect[ie];
        auto elem_coord = get_values(coords, idx);
        auto volume = volumes(ie);
        grad_shfns(ie) = grad_shape<ELEM_TYPE, DIM>(elem_coord, volume);
    }
    return grad_shfns;
}

template <ElementType ELEM_TYPE, Int DIM, Int N_ELEM_NODES = get_num_element_nodes(ELEM_TYPE)>
inline Array1D<DenseMatrix<Real, DIM, N_ELEM_NODES>>
calc_grad_shape(const UnstructuredMesh & mesh, const Array1D<Real> & volumes)
{
    CALL_STACK_MSG();
    Int n_elems = mesh.get_num_cells();
    Array1D<DenseMatrix<Real, DIM, N_ELEM_NODES>> grad_shfns(n_elems);
    auto dm = mesh.get_coordinate_dm();
    auto vec = mesh.get_coordinates_local();
    auto section = mesh.get_coordinate_section();
    DenseMatrix<Real, N_ELEM_NODES, DIM> elem_coord;
    Int sz = DIM * N_ELEM_NODES;
    for (Int ie = 0; ie < n_elems; ++ie) {
        Real * data = elem_coord.data();
        PETSC_CHECK(DMPlexVecGetClosure(dm, section, vec, ie, &sz, &data));
        auto volume = volumes(ie);
        grad_shfns(ie) = grad_shape<ELEM_TYPE, DIM>(elem_coord, volume);
    }
    return grad_shfns;
}

} // namespace fe

} // namespace godzilla
