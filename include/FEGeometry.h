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
void
coordinates(const UnstructuredMesh & mesh, Array1D<DenseVector<Real, DIM>> & coords)
{
    _F_;
    assert(coords.get_size() == mesh.get_num_vertices());
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
}

template <Int DIM, Int N_ELEM_NODES>
void
connectivity(const UnstructuredMesh & mesh, Array1D<DenseVector<Int, N_ELEM_NODES>> & connect)
{
    _F_;
    assert(connect.get_size() == mesh.get_num_cells());
    for (auto elem_id : mesh.get_all_cell_range()) {
        connect(elem_id) = DenseVector<Int, N_ELEM_NODES>(mesh.get_cell_connectivity(elem_id));
    }
}

// Normals

template <ElementType ELEM_TYPE, Int DIM>
DenseVector<Real, DIM>
normal(Real volume, Real edge_len, const DenseVector<Real, DIM> & grad)
{
    _F_;
    error("Computation of a normal for element '{}' in {} dimensions is not implemented.",
          get_element_type_str(ELEM_TYPE),
          DIM);
}

template <>
DenseVector<Real, 1>
normal<EDGE2, 1>(Real volume, Real edge_len, const DenseVector<Real, 1> & grad)
{
    return -volume * grad;
}

template <>
DenseVector<Real, 2>
normal<TRI3, 2>(Real volume, Real edge_len, const DenseVector<Real, 2> & grad)
{
    return -2. * volume / edge_len * grad;
}

// Element lengths

template <ElementType ELEM_TYPE, Int DIM, Int N_ELEM_NODES = get_num_element_nodes(ELEM_TYPE)>
Real
element_length(const DenseVector<DenseVector<Real, DIM>, N_ELEM_NODES> & grad_phi)
{
    _F_;
    error("Computation of a element length for '{}' in {} dimensions is not implemented.",
          get_element_type_str(ELEM_TYPE),
          DIM);
}

template <>
Real
element_length<EDGE2, 1, 2>(const DenseVector<DenseVector<Real, 1>, 2> & grad_phi)
{
    auto h1 = 1. / grad_phi(0).magnitude();
    auto h2 = 1. / grad_phi(1).magnitude();
    return std::min({ h1, h2 });
}

template <>
Real
element_length<TRI3, 2, 3>(const DenseVector<DenseVector<Real, 2>, 3> & grad_phi)
{
    auto h1 = 1. / grad_phi(0).magnitude();
    auto h2 = 1. / grad_phi(1).magnitude();
    auto h3 = 1. / grad_phi(2).magnitude();
    return std::min({ h1, h2, h3 });
}

template <ElementType ELEM_TYPE, Int DIM, Int N_ELEM_NODES = get_num_element_nodes(ELEM_TYPE)>
void
calc_element_length(const Array1D<DenseVector<Int, N_ELEM_NODES>> & connect,
                    const Array2D<DenseVector<Real, DIM>> & grad_phi,
                    Array1D<Real> & elem_lengths)
{
    _F_;
    for (godzilla::Int ie = 0; ie < connect.get_size(); ie++) {
        DenseVector<DenseVector<Real, DIM>, N_ELEM_NODES> dndx;
        for (Int i = 0; i < N_ELEM_NODES; i++)
            dndx(i) = grad_phi(ie, i);
        elem_lengths(ie) = element_length<ELEM_TYPE, DIM>(dndx);
    }
}

} // namespace fe

} // namespace godzilla
