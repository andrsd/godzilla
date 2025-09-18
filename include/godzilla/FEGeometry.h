// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/CallStack.h"
#include "godzilla/Types.h"
#include "godzilla/Numerics.h"
#include "godzilla/Convert.h"
#include "godzilla/Exception.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/Vector.h"
#include "godzilla/Array1D.h"
#include "godzilla/DenseVector.h"
#include "godzilla/DenseMatrix.h"
#include <set>

namespace godzilla {

namespace fe {

/// Convert coordinates from unstructured mesh into an array of `DenseVector`s
///
/// @tparam DIM Spatial dimension
/// @param mesh Unstructured mesh
/// @param coords Computed coordinates
template <Int DIM>
Array1D<DenseVector<Real, DIM>>
coordinates(const UnstructuredMesh & mesh)
{
    auto vtx_range = mesh.get_vertex_range();
    Array1D<DenseVector<Real, DIM>> coords(vtx_range);
    Vector vc = mesh.get_coordinates_local();
    Scalar * coord_vals = vc.get_array();
    Int j = 0;
    for (auto & vtx : vtx_range) {
        DenseVector<Real, DIM> c;
        for (Int i = 0; i < DIM; ++i, ++j)
            c(i) = coord_vals[j];
        coords(vtx) = c;
    }
    vc.restore_array(coord_vals);
    return coords;
}

template <Int DIM, Int N_ELEM_NODES>
Array1D<DenseVector<Int, N_ELEM_NODES>>
connectivity(const UnstructuredMesh & mesh)
{
    CALL_STACK_MSG();
    auto n_cells = mesh.get_num_cells();
    Array1D<DenseVector<Int, N_ELEM_NODES>> connect(n_cells);
    for (auto elem_id : mesh.get_cell_range()) {
        auto cell_conn = mesh.get_connectivity(elem_id);
        for (Int i = 0; i < N_ELEM_NODES; ++i)
            connect(elem_id)(i) = cell_conn[i];
    }
    return connect;
}

/// Compute node-element connectivity
///
/// @param mesh Unstructured mesh
/// @return Array describing which elements are connected to which nodes
///        Indexing: array[node_id][el0, el1, ... ].
///        Nodes and cells are using zero-based indexing
template <Int N_ELEM_NODES>
inline Array1D<std::vector<Int>>
common_elements_by_node(const UnstructuredMesh & mesh)
{
    CALL_STACK_MSG();
    auto n_all_cells = mesh.get_num_all_cells();
    auto n_nodes = mesh.get_num_vertices();
    Array1D<std::vector<Int>> nelcom(n_nodes);
    for (auto & cell : mesh.get_cell_range()) {
        const auto & node_ids = mesh.get_connectivity(cell);
        for (Int j = 0; j < N_ELEM_NODES; ++j)
            nelcom(node_ids[j] - n_all_cells).push_back(cell);
    }
    return nelcom;
}

// Normals

template <ElementType ELEM_TYPE, Int DIM>
inline DenseVector<Real, DIM>
normal(Real volume, Real face_volume, const DenseVector<Real, DIM> & grad)
{
    CALL_STACK_MSG();
    throw NotImplementedException(
        "Computation of a normal for element '{}' in {} dimensions is not implemented",
        conv::to_str(ELEM_TYPE),
        DIM);
}

template <>
inline DenseVector<Real, 1>
normal<EDGE2, 1>(Real volume, Real face_volume, const DenseVector<Real, 1> & grad)
{
    return -volume * grad;
}

template <>
inline DenseVector<Real, 2>
normal<TRI3, 2>(Real volume, Real face_volume, const DenseVector<Real, 2> & grad)
{
    auto c = -2. * volume / face_volume;
    return c * grad;
}

template <>
inline DenseVector<Real, 3>
normal<TET4, 3>(Real volume, Real face_volume, const DenseVector<Real, 3> & grad)
{
    auto c = -3. * volume / face_volume;
    return c * grad;
}

// Element lengths

template <ElementType ELEM_TYPE, Int DIM, Int N_ELEM_NODES = get_num_element_nodes(ELEM_TYPE)>
inline Real
element_length(const DenseMatrix<Real, DIM, N_ELEM_NODES> & grad_phi)
{
    CALL_STACK_MSG();
    throw NotImplementedException(
        "Computation of a element length for '{}' in {} dimensions is not implemented",
        conv::to_str(ELEM_TYPE),
        DIM);
}

template <>
inline Real
element_length<EDGE2, 1, 2>(const DenseMatrix<Real, 1, 2> & grad_phi)
{
    auto h1 = 1. / math::abs(grad_phi(0, 0));
    auto h2 = 1. / math::abs(grad_phi(0, 1));
    return math::min({ h1, h2 });
}

template <>
inline Real
element_length<TRI3, 2, 3>(const DenseMatrix<Real, 2, 3> & grad_phi)
{
    Real h[3];
    for (int i = 0; i < 3; ++i) {
        DenseVector<Real, 2> v(grad_phi.column(i));
        h[i] = 1. / v.magnitude();
    }
    return math::min({ h[0], h[1], h[2] });
}

template <ElementType ELEM_TYPE, Int DIM, Int N_ELEM_NODES = get_num_element_nodes(ELEM_TYPE)>
Array1D<Real>
calc_element_length(const Array1D<DenseMatrix<Real, DIM, N_ELEM_NODES>> & grad_phi)
{
    CALL_STACK_MSG();
    Array1D<Real> elem_lengths(grad_phi.size());
    for (Int ie = 0; ie < grad_phi.size(); ++ie)
        elem_lengths(ie) = element_length<ELEM_TYPE, DIM>(grad_phi(ie));
    return elem_lengths;
}

//

template <CoordinateType COORD_TYPE, Int DIM>
inline Array1D<Real>
calc_nodal_radius(const Array1D<DenseVector<Real, DIM>> & coords)
{
    throw NotImplementedException("Radius computation is not implemented in {} dimensions", DIM);
}

template <>
inline Array1D<Real>
calc_nodal_radius<CARTESIAN, 1>(const Array1D<DenseVector<Real, 1>> & coords)
{
    CALL_STACK_MSG();
    auto n = coords.size();
    Array1D<Real> rad(n);
    for (Int in = 0; in < n; ++in)
        rad(in) = 1.;
    return rad;
}

template <>
inline Array1D<Real>
calc_nodal_radius<CARTESIAN, 2>(const Array1D<DenseVector<Real, 2>> & coords)
{
    CALL_STACK_MSG();
    auto n = coords.size();
    Array1D<Real> rad(n);
    for (Int in = 0; in < n; ++in)
        rad(in) = 1.;
    return rad;
}

template <>
inline Array1D<Real>
calc_nodal_radius<CARTESIAN, 3>(const Array1D<DenseVector<Real, 3>> & coords)
{
    CALL_STACK_MSG();
    auto n = coords.size();
    Array1D<Real> rad(n);
    for (Int in = 0; in < n; ++in)
        rad(in) = 1.;
    return rad;
}

template <>
inline Array1D<Real>
calc_nodal_radius<AXISYMMETRIC, 2>(const Array1D<DenseVector<Real, 2>> & coords)
{
    // symmetric around x-axis
    CALL_STACK_MSG();
    auto n = coords.size();
    Array1D<Real> rad(n);
    for (Int in = 0; in < n; ++in)
        rad(in) = coords(in)(1);
    return rad;
}

/// Get local face index
///
/// @param elem_conn Element connectivity array
/// @param face_conn Face connectivity array
/// @return Local face index
inline Int
get_local_face_index(const std::vector<Int> & elem_conn, const std::vector<Int> & face_conn)
{
    // this only works for simplexes
    std::set<Int> ec(elem_conn.begin(), elem_conn.end());
    std::set<Int> fc(face_conn.begin(), face_conn.end());
    std::vector<Int> diff;
    std::set_difference(ec.begin(),
                        ec.end(),
                        fc.begin(),
                        fc.end(),
                        std::inserter(diff, diff.begin()));
    assert(diff.size() == 1);
    auto it = std::find(elem_conn.begin(), elem_conn.end(), diff[0]);
    return std::distance(elem_conn.begin(), it);
}

template <ElementType ELEM_TYPE, Int DIM, Int N_ELEM_NODES = get_num_element_nodes(ELEM_TYPE)>
inline Int
get_grad_fn_index(Int facet_idx)
{
    throw NotImplementedException(
        "get_grad_fn_index is not implemented for {} element in {} dimensions",
        conv::to_str(ELEM_TYPE),
        DIM);
}

template <>
inline Int
get_grad_fn_index<EDGE2, 1, 2>(Int facet_idx)
{
    static std::array<Int, 2> grad_fn_index = { 1, 0 };
    assert(facet_idx >= 0 && facet_idx < 2);
    return grad_fn_index[facet_idx];
}

template <>
inline Int
get_grad_fn_index<TRI3, 2, 3>(Int facet_idx)
{
    static std::array<Int, 3> grad_fn_index = { 2, 0, 1 };
    assert(facet_idx >= 0 && facet_idx < 3);
    return grad_fn_index[facet_idx];
}

template <>
inline Int
get_grad_fn_index<TET4, 3, 4>(Int facet_idx)
{
    static std::array<Int, 4> grad_fn_index = { 3, 2, 1, 0 };
    assert(facet_idx >= 0 && facet_idx < 4);
    return grad_fn_index[facet_idx];
}

} // namespace fe

} // namespace godzilla
