#pragma once

#include "Types.h"
#include "Error.h"
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
DenseMatrixSymm<Real, N_ELEM_NODES>
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

// Stiffness matrices

/// Template for local stiffness matrix in `DIM` spatial dimensions for elements with `N_ELEM_NODES`
///
/// @tparam ETYPE Element type
/// @tparam N_ELEM_NODES Number of element nodes
/// @return `DenseMatrixSymm` containing local stiffness matrix
template <ElementType ETYPE, Int N_ELEM_NODES = get_num_element_nodes(ETYPE)>
DenseMatrixSymm<Real, N_ELEM_NODES>
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

} // namespace matrix

} // namespace fe

} // namespace godzilla
