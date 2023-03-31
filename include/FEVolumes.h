#pragma once

#include "CallStack.h"
#include "Types.h"
#include "Array1D.h"
#include "DenseVector.h"

namespace godzilla {

namespace fe {

template <ElementType ELEM_TYPE, Int DIM, Int N_ELEM_NODES = get_num_element_nodes(ELEM_TYPE)>
Real
volume(const DenseVector<DenseVector<Real, DIM>, N_ELEM_NODES> & coords)
{
    _F_;
    error("Volume calculation for {} in {} dimensions is not implemented.",
          get_element_type_str(ELEM_TYPE),
          DIM);
}

/// Compute volume of a EDGE2 in 1D
template <>
inline Real
volume<EDGE2, 1>(const DenseVector<DenseVector<Real, 1>, 2> & coords)
{
    Real vol = std::abs(coords(0)(0) - coords(1)(0));
    return vol;
}

/// Compute volume of a EDGE2 in 2D
template <>
inline Real
volume<EDGE2, 2>(const DenseVector<DenseVector<Real, 2>, 2> & coords)
{
    DenseVector<Real, 2> v;
    v(0) = coords(0)(0) - coords(1)(0);
    v(1) = coords(0)(1) - coords(1)(1);
    return v.magnitude();
}

/// Compute volume of a TRI3 in 2D
template <>
inline Real
volume<TRI3, 2>(const DenseVector<DenseVector<Real, 2>, 3> & coords)
{
    // Element volume = half the determinant of the coordinate Jacobian
    Real X1 = coords(0)(0);
    Real Y1 = coords(0)(1);

    Real X2 = coords(1)(0);
    Real Y2 = coords(1)(1);

    Real X3 = coords(2)(0);
    Real Y3 = coords(2)(1);

    return 0.5 * (X2 * Y3 - X3 * Y2 - X1 * (Y3 - Y2) + Y1 * (X3 - X2));
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
    _F_;
    assert(connect.get_size() == fe_volume.get_size());

    for (godzilla::Int ie = 0; ie < connect.get_size(); ie++) {
        auto idx = connect(ie);
        auto elem_coord = coords.get_values(idx);
        fe_volume(ie) = volume<ELEM_TYPE, DIM>(elem_coord);
    }
}

template <ElementType ELEM_TYPE, Int DIM, Int N_ELEM_NODES = get_num_element_nodes(ELEM_TYPE)>
Array1D<Real>
calc_volumes(const Array1D<DenseVector<Real, DIM>> & coords,
             const Array1D<DenseVector<Int, N_ELEM_NODES>> & connect)
{
    _F_;
    Array1D<Real> fe_volume(connect.get_size());
    calc_volumes<ELEM_TYPE, DIM, N_ELEM_NODES>(coords, connect, fe_volume);
    return fe_volume;
}

} // namespace fe

} // namespace godzilla
