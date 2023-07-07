#pragma once

#include "CallStack.h"
#include "Types.h"
#include "UnstructuredMesh.h"
#include "Vector.h"
#include "Array1D.h"
#include "Array2D.h"
#include "DenseVector.h"

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
    Array1D<DenseVector<Real, DIM>> coords(mesh.get_num_vertices());
    Vector vc = mesh.get_coordinates_local();
    auto vtx_range = mesh.get_vertex_range();
    Scalar * coord_vals = vc.get_array();
    for (Int n = 0, j = 0; n < vtx_range.size(); n++) {
        DenseVector<Real, DIM> c;
        for (Int i = 0; i < DIM; i++, j++)
            c(i) = coord_vals[j];
        coords(n) = c;
    }
    vc.restore_array(coord_vals);
    return coords;
}

template <Int DIM, Int N_ELEM_NODES>
Array1D<DenseVector<Int, N_ELEM_NODES>>
connectivity(const UnstructuredMesh & mesh)
{
    _F_;
    Array1D<DenseVector<Int, N_ELEM_NODES>> connect(mesh.get_num_cells());
    for (auto elem_id : mesh.get_cell_range())
        connect(elem_id) = DenseVector<Int, N_ELEM_NODES>(mesh.get_cell_connectivity(elem_id));
    return connect;
}

template <Int N_ELEM_NODES>
void
common_elements_by_node(const Array1D<DenseVector<Int, N_ELEM_NODES>> & connect,
                        Array1D<std::vector<Int>> & nelcom)
{
    _F_;
    for (Int ei = 0; ei < connect.get_size(); ei++) {
        const auto & node_ids = connect(ei);
        for (Int j = 0; j < N_ELEM_NODES; j++)
            nelcom(node_ids(j)).push_back(ei);
    }
}

// Normals

template <ElementType ELEM_TYPE, Int DIM>
inline DenseVector<Real, DIM>
normal(Real volume, Real edge_len, const DenseVector<Real, DIM> & grad)
{
    _F_;
    error("Computation of a normal for element '{}' in {} dimensions is not implemented.",
          get_element_type_str(ELEM_TYPE),
          DIM);
}

template <>
inline DenseVector<Real, 1>
normal<EDGE2, 1>(Real volume, Real edge_len, const DenseVector<Real, 1> & grad)
{
    return -volume * grad;
}

template <>
inline DenseVector<Real, 2>
normal<TRI3, 2>(Real volume, Real edge_len, const DenseVector<Real, 2> & grad)
{
    return -2. * volume / edge_len * grad;
}

// Element lengths

template <ElementType ELEM_TYPE, Int DIM, Int N_ELEM_NODES = get_num_element_nodes(ELEM_TYPE)>
inline Real
element_length(const DenseVector<DenseVector<Real, DIM>, N_ELEM_NODES> & grad_phi)
{
    _F_;
    error("Computation of a element length for '{}' in {} dimensions is not implemented.",
          get_element_type_str(ELEM_TYPE),
          DIM);
}

template <>
inline Real
element_length<EDGE2, 1, 2>(const DenseVector<DenseVector<Real, 1>, 2> & grad_phi)
{
    auto h1 = 1. / grad_phi(0).magnitude();
    auto h2 = 1. / grad_phi(1).magnitude();
    return std::min({ h1, h2 });
}

template <>
inline Real
element_length<TRI3, 2, 3>(const DenseVector<DenseVector<Real, 2>, 3> & grad_phi)
{
    auto h1 = 1. / grad_phi(0).magnitude();
    auto h2 = 1. / grad_phi(1).magnitude();
    auto h3 = 1. / grad_phi(2).magnitude();
    return std::min({ h1, h2, h3 });
}

template <ElementType ELEM_TYPE, Int DIM, Int N_ELEM_NODES = get_num_element_nodes(ELEM_TYPE)>
Array1D<Real>
calc_element_length(const Array1D<DenseVector<Int, N_ELEM_NODES>> & connect,
                    const Array1D<DenseVector<DenseVector<Real, DIM>, N_ELEM_NODES>> & grad_phi)
{
    _F_;
    Array1D<Real> elem_lengths(connect.get_size());
    for (godzilla::Int ie = 0; ie < connect.get_size(); ie++)
        elem_lengths(ie) = element_length<ELEM_TYPE, DIM>(grad_phi(ie));
    return elem_lengths;
}

//

template <CoordinateType COORD_TYPE, Int DIM>
inline Array1D<Real>
calc_nodal_radius(Array1D<DenseVector<Real, DIM>> & coords)
{
    error("Radius computation is not implemented in {} dimensions.", DIM);
}

template <>
inline Array1D<Real>
calc_nodal_radius<CARTESIAN, 1>(Array1D<DenseVector<Real, 1>> & coords)
{
    _F_;
    auto n = coords.get_size();
    Array1D<Real> rad(n);
    for (Int in = 0; in < n; in++)
        rad(in) = 1.;
    return rad;
}

template <>
inline Array1D<Real>
calc_nodal_radius<CARTESIAN, 2>(Array1D<DenseVector<Real, 2>> & coords)
{
    _F_;
    auto n = coords.get_size();
    Array1D<Real> rad(n);
    for (Int in = 0; in < n; in++)
        rad(in) = 1.;
    return rad;
}

template <>
inline Array1D<Real>
calc_nodal_radius<CARTESIAN, 3>(Array1D<DenseVector<Real, 3>> & coords)
{
    _F_;
    auto n = coords.get_size();
    Array1D<Real> rad(n);
    for (Int in = 0; in < n; in++)
        rad(in) = 1.;
    return rad;
}

template <>
inline Array1D<Real>
calc_nodal_radius<AXISYMMETRIC, 2>(Array1D<DenseVector<Real, 2>> & coords)
{
    // symmetric around x-axis
    _F_;
    auto n = coords.get_size();
    Array1D<Real> rad(n);
    for (Int in = 0; in < n; in++)
        rad(in) = coords(in)(1);
    return rad;
}

} // namespace fe

} // namespace godzilla
