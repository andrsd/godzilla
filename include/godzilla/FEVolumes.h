// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/CallStack.h"
#include "godzilla/Types.h"
#include "godzilla/Array1D.h"
#include "godzilla/DenseVector.h"
#include "godzilla/DenseMatrix.h"
#include "godzilla/UnstructuredMesh.h"
#include "petscdmplex.h"

namespace godzilla {

namespace fe {

template <ElementType ELEM_TYPE, Int DIM, Int N_ELEM_NODES = get_num_element_nodes(ELEM_TYPE)>
Real
volume(const DenseMatrix<Real, N_ELEM_NODES, DIM> & coords)
{
    CALL_STACK_MSG();
    throw NotImplementedException("Volume calculation for {} in {} dimensions is not implemented",
                                  get_element_type_str(ELEM_TYPE),
                                  DIM);
}

/// Compute volume of a EDGE2 in 1D
template <>
inline Real
volume<EDGE2, 1>(const DenseMatrix<Real, 2, 1> & coords)
{
    Real vol = std::abs(coords(0, 0) - coords(1, 0));
    return vol;
}

/// Compute volume of a EDGE2 in 2D
template <>
inline Real
volume<EDGE2, 2>(const DenseMatrix<Real, 2, 2> & coords)
{
    DenseVector<Real, 2> v;
    v(0) = coords(0, 0) - coords(1, 0);
    v(1) = coords(0, 1) - coords(1, 1);
    return v.magnitude();
}

/// Compute volume of a TRI3 in 2D
template <>
inline Real
volume<TRI3, 2>(const DenseMatrix<Real, 3, 2> & coords)
{
    // Element volume = half the determinant of the coordinate Jacobian
    auto x1 = coords(0, 0);
    auto y1 = coords(0, 1);

    auto x2 = coords(1, 0);
    auto y2 = coords(1, 1);

    auto x3 = coords(2, 0);
    auto y3 = coords(2, 1);

    return 0.5 * (x2 * y3 - x3 * y2 - x1 * (y3 - y2) + y1 * (x3 - x2));
}

/// Compute volume of a TET4 in 3D
template <>
inline Real
volume<TET4, 3>(const DenseMatrix<Real, 4, 3> & coords)
{
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

    DenseVector<Real, 3> v0({ x2 - x1, y2 - y1, z2 - z1 });
    DenseVector<Real, 3> v1({ x3 - x1, y3 - y1, z3 - z1 });
    DenseVector<Real, 3> v2({ x4 - x1, y4 - y1, z4 - z1 });

    return (1. / 6.) * dot(cross_product(v0, v1), v2);
}

/// Compute FE volumes
///
/// @tparam ELEM_TYPE Element type
/// @tparam DIM Spatial dimension
/// @tparam N_ELEM_NODES Number of nodes per element
/// @param coords Array with coordinates
/// @param connect Connectivity array
/// @param fe_volume Computed FE volumes
template <ElementType ELEM_TYPE, Int DIM, Int N_ELEM_NODES = get_num_element_nodes(ELEM_TYPE)>
void
calc_volumes(const Array1D<DenseVector<Real, DIM>> & coords,
             const Array1D<DenseVector<Int, N_ELEM_NODES>> & connect,
             Array1D<Real> & fe_volume)
{
    CALL_STACK_MSG();
    assert(connect.get_size() == fe_volume.get_size());

    for (godzilla::Int ie = 0; ie < connect.get_size(); ++ie) {
        auto idx = connect(ie);
        auto elem_coord = mat_row(coords.get_values(idx));
        fe_volume(ie) = volume<ELEM_TYPE, DIM>(elem_coord);
    }
}

template <ElementType ELEM_TYPE, Int DIM, Int N_ELEM_NODES = get_num_element_nodes(ELEM_TYPE)>
Array1D<Real>
calc_volumes(const Array1D<DenseVector<Real, DIM>> & coords,
             const Array1D<DenseVector<Int, N_ELEM_NODES>> & connect)
{
    CALL_STACK_MSG();
    Array1D<Real> fe_volume(connect.get_size());
    calc_volumes<ELEM_TYPE, DIM, N_ELEM_NODES>(coords, connect, fe_volume);
    return fe_volume;
}

template <ElementType ELEM_TYPE, Int DIM, Int N_ELEM_NODES = get_num_element_nodes(ELEM_TYPE)>
Array1D<Real>
calc_volumes(const UnstructuredMesh & mesh)
{
    CALL_STACK_MSG();
    Int n_elems = mesh.get_num_cells();
    Array1D<Real> vols(n_elems);
    auto dm = mesh.get_coordinate_dm();
    auto vec = mesh.get_coordinates_local();
    auto section = mesh.get_coordinate_section();
    DenseMatrix<Real, N_ELEM_NODES, DIM> elem_coord;
    Int sz = DIM * N_ELEM_NODES;
    for (godzilla::Int ie = 0; ie < n_elems; ++ie) {
        Real * data = elem_coord.data();
        PETSC_CHECK(DMPlexVecGetClosure(dm, section, vec, ie, &sz, &data));
        vols(ie) = fe::volume<ELEM_TYPE, DIM>(elem_coord);
    }
    return vols;
}

//

template <ElementType ELEM_TYPE, Int DIM, Int N_FACE_NODES>
Real
face_area(const DenseMatrix<Real, N_FACE_NODES, DIM> & coords)
{
    CALL_STACK_MSG();
    throw NotImplementedException(
        "Face area calculation for {} in {} dimensions is not implemented",
        get_element_type_str(ELEM_TYPE),
        DIM);
}

template <>
inline Real
face_area<EDGE2, 1>(const DenseMatrix<Real, 1, 1> & coords)
{
    return 1.;
}

template <>
inline Real
face_area<TRI3, 2>(const DenseMatrix<Real, 2, 2> & coords)
{
    DenseVector<Real, 2> v;
    v(0) = coords(0, 0) - coords(1, 0);
    v(1) = coords(0, 1) - coords(1, 1);
    return v.magnitude();
}

} // namespace fe

} // namespace godzilla
