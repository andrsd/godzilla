#pragma once

#include "Types.h"
#include "Error.h"

namespace godzilla {

namespace fe {

// Linear forms

/// Get coefficient for exact integration of a linear form
///
/// @tparam ELEM_TYPE Element type
/// @tparam N1 Order of the shape function
/// @return
template <ElementType ELEM_TYPE, Int N1>
const Real
integration_coeff()
{
    error("Integration coefficient for '{}' is not implemented", get_element_type_str(ELEM_TYPE));
}

template <>
const Real
integration_coeff<EDGE2, 1>()
{
    return 1. / 2.;
}

template <>
const Real
integration_coeff<TRI3, 1>()
{
    return 1. / 3.;
}

template <>
const Real
integration_coeff<TET4, 1>()
{
    return 1. / 4.;
}

// Bilinear forms

/// Get coefficient for exact integration of a bilinear form
///
/// @tparam ELEM_TYPE Element type
/// @tparam N1 Order of the 1st shape function
/// @tparam N1 Order of the 2nd shape function
/// @return
template <ElementType ELEM_TYPE, Int N1, Int N2>
const Real
integration_coeff()
{
    error("Integration coefficient for '{}' is not implemented", get_element_type_str(ELEM_TYPE));
}

template <>
const Real
integration_coeff<EDGE2, 1, 1>()
{
    return 1. / 6.;
}

template <>
const Real
integration_coeff<TRI3, 1, 1>()
{
    return 1. / 12.;
}

template <>
const Real
integration_coeff<TET4, 1, 1>()
{
    return 1. / 20.;
}

// these are fudge factors

template <>
const Real
integration_coeff<EDGE2, 0, 0>()
{
    return 1.;
}

template <>
const Real
integration_coeff<TRI3, 0, 0>()
{
    return 1. / 4.;
}

} // namespace fe

} // namespace godzilla
