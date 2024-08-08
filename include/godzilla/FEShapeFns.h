// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/CallStack.h"
#include "godzilla/Types.h"
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
    error("Calculation of shape function gradients is not implemented for element '{}' yet.",
          get_element_type_str(ELEM_TYPE));
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

    Real x31 = x3 - x1;
    Real x21 = x2 - x1;
    Real y31 = y3 - y1;
    Real y21 = y2 - y1;

    Real c = 0.5 / volume;
    DenseMatrix<Real, 2, 3> grads;
    grads.set_col(0, { c * (y21 - y31), c * (x31 - x21) });
    grads.set_col(1, { c * y31, -c * x31 });
    grads.set_col(2, { -c * y21, c * x21 });
    return grads;
}

template <ElementType ELEM_TYPE, Int DIM, Int N_ELEM_NODES = get_num_element_nodes(ELEM_TYPE)>
inline Array1D<DenseMatrix<Real, DIM, N_ELEM_NODES>>
calc_grad_shape(const Array1D<DenseVector<Real, DIM>> & coords,
                const Array1D<DenseVector<Int, N_ELEM_NODES>> & connect,
                const Array1D<Real> & volumes)
{
    CALL_STACK_MSG();
    Array1D<DenseMatrix<Real, DIM, N_ELEM_NODES>> grad_shfns(connect.get_size());
    for (Int ie = 0; ie < connect.get_size(); ie++) {
        auto idx = connect.get(ie);
        auto elem_coord = mat_row(coords.get_values(idx));
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
    for (Int ie = 0; ie < n_elems; ie++) {
        Real * data = elem_coord.data();
        PETSC_CHECK(DMPlexVecGetClosure(dm, section, vec, ie, &sz, &data));
        auto volume = volumes(ie);
        grad_shfns(ie) = grad_shape<ELEM_TYPE, DIM>(elem_coord, volume);
    }
    return grad_shfns;
}

} // namespace fe

} // namespace godzilla
