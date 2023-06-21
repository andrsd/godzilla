#pragma once

#include "Types.h"
#include "Error.h"
#include "DenseVector.h"
#include "DenseMatrix.h"
#include "DenseMatrixSymm.h"

namespace godzilla {

namespace fe {

namespace matrix {

// Mass matrices

/// Template for local mass matrix in `DIM` spatial dimensions for elements with `N_ELEM_NODES`
///
/// @tparam ETYPE Element type
/// @tparam N_ELEM_NODES Number of element nodes
/// @return `DenseMatrixSymm` containing local mass matrix
template <ElementType ETYPE, Int N_ELEM_NODES = get_num_element_nodes(ETYPE)>
inline DenseMatrixSymm<Real, N_ELEM_NODES>
mass()
{
    error("Mass matrix in not implemented for {}.", get_element_type_str(ETYPE));
}

/// Local mass matrix for EDGE2 in 1D
template <>
inline DenseMatrixSymm<Real, 2>
mass<EDGE2>()
{
    DenseMatrixSymm<Real, 2> m;
    m(0, 0) = 2.;
    m(0, 1) = 1.;
    m(1, 1) = 2.;
    return m;
}

/// Local mass matrix for TRI3 in 2D
template <>
inline DenseMatrixSymm<Real, 3>
mass<TRI3>()
{
    DenseMatrixSymm<Real, 3> m;
    m(0, 0) = 2.;
    m(0, 1) = 1.;
    m(0, 2) = 1.;
    m(1, 1) = 2.;
    m(1, 2) = 1.;
    m(2, 2) = 2.;
    return m;
}

/// Local mass matrix for TET4 in 3D
template <>
inline DenseMatrixSymm<Real, 4>
mass<TET4>()
{
    DenseMatrixSymm<Real, 4> m;
    m(0, 0) = 2.;
    m(0, 1) = 1.;
    m(0, 2) = 1.;
    m(0, 3) = 1.;
    m(1, 1) = 2.;
    m(1, 2) = 1.;
    m(1, 3) = 1.;
    m(2, 2) = 2.;
    m(2, 3) = 1.;
    m(3, 3) = 2.;
    return m;
}

//

template <ElementType ETYPE, Int N_ELEM_NODES = get_num_element_nodes(ETYPE)>
inline DenseMatrixSymm<Real, N_ELEM_NODES>
mass_rz(Real rad_e, const DenseVector<Real, N_ELEM_NODES> & rad_n)
{
    error("Mass matrix (RZ) in not implemented for {}.", get_element_type_str(ETYPE));
}

/// Local mass matrix (RZ) for EDGE2 in 1D
template <>
inline DenseMatrixSymm<Real, 2>
mass_rz<EDGE2>(Real rad_e, const DenseVector<Real, 2> & rad_n)
{
    DenseMatrixSymm<Real, 2> m;
    m(0, 0) = (4 * rad_e + 4. * rad_n(0));
    m(0, 1) = (4 * rad_e);
    m(1, 1) = (4 * rad_e + 4. * rad_n(1));
    return m;
}

/// Local mass matrix (RZ) for TRI3 in 2D
template <>
inline DenseMatrixSymm<Real, 3>
mass_rz<TRI3>(Real rad_e, const DenseVector<Real, 3> & rad_n)
{
    DenseMatrixSymm<Real, 3> m;
    m(0, 0) = 6. * rad_e + 4. * rad_n(0);
    m(0, 1) = 6. * rad_e - rad_n(2);
    m(0, 2) = 6. * rad_e - rad_n(1);
    m(1, 1) = 6. * rad_e + 4. * rad_n(1);
    m(1, 2) = 6. * rad_e - rad_n(0);
    m(2, 2) = 6. * rad_e + 4. * rad_n(2);
    return m;
}

// Stiffness matrices

/// Template for local stiffness matrix in `DIM` spatial dimensions for elements with `N_ELEM_NODES`
///
/// @tparam ETYPE Element type
/// @tparam N_ELEM_NODES Number of element nodes
/// @return `DenseMatrixSymm` containing local stiffness matrix
template <ElementType ETYPE, Int N_ELEM_NODES = get_num_element_nodes(ETYPE)>
inline DenseMatrixSymm<Real, N_ELEM_NODES>
stiffness()
{
    error("Stiffness matrix in not implemented for {}.", get_element_type_str(ETYPE));
}

/// Local mass matrix for EDGE2 in 1D
template <>
inline DenseMatrixSymm<Real, 2>
stiffness<EDGE2>()
{
    DenseMatrixSymm<Real, 2> m;
    m(0, 0) = 1.;
    m(0, 1) = -1.;
    m(1, 1) = 1.;
    return m;
}

/// Local mass matrix for TRI3 in 1D
template <>
inline DenseMatrixSymm<Real, 3>
stiffness<TRI3>()
{
    DenseMatrixSymm<Real, 3> m;
    m(0, 0) = 1.;
    m(0, 1) = -1.;
    m(0, 2) = 0.;
    m(1, 1) = 2.;
    m(1, 2) = -1.;
    m(2, 2) = 1.;
    return m;
}

/// Local mass matrix for TET4 in 1D
template <>
inline DenseMatrixSymm<Real, 4>
stiffness<TET4>()
{
    DenseMatrixSymm<Real, 4> m;
    m(0, 0) = 1.;
    m(0, 1) = -1.;
    m(0, 2) = 0.;
    m(0, 3) = 0.;
    m(1, 1) = 2.;
    m(1, 2) = -1.;
    m(1, 3) = 0.;
    m(2, 2) = 2.;
    m(2, 3) = -1.;
    m(3, 3) = 1.;
    return m;
}

//

template <ElementType ELEM_TYPE, Int N_BND_NODES>
inline DenseMatrix<Real, N_BND_NODES>
bnd_rz(const DenseVector<Real, N_BND_NODES> & radii)
{
    error("Not implemented.");
}

template <>
inline DenseMatrix<Real, 1>
bnd_rz<EDGE2, 1>(const DenseVector<Real, 1> & radius)
{
    DenseMatrix<Real, 1> bi;
    bi(0, 0) = 1.;
    return bi;
}

template <>
inline DenseMatrix<Real, 2>
bnd_rz<TRI3, 2>(const DenseVector<Real, 2> & radius)
{
    DenseMatrix<Real, 2> bi;
    bi(0, 0) = 3. * radius(0) + radius(1);
    bi(0, 1) = radius(0) + radius(1);
    bi(1, 0) = radius(0) + radius(1);
    bi(1, 1) = radius(0) + 3. * radius(1);
    return bi;
}

} // namespace matrix

} // namespace fe

} // namespace godzilla
