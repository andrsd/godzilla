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
inline Real
integration_coeff()
{
    error("Integration coefficient for '{}' is not implemented", get_element_type_str(ELEM_TYPE));
}

template <>
inline Real
integration_coeff<EDGE2, 1>()
{
    return 1. / 2.;
}

template <>
inline Real
integration_coeff<TRI3, 1>()
{
    return 1. / 3.;
}

template <>
inline Real
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
inline Real
integration_coeff()
{
    error("Integration coefficient for '{}' is not implemented", get_element_type_str(ELEM_TYPE));
}

template <>
inline Real
integration_coeff<EDGE2, 1, 1>()
{
    return 1. / 6.;
}

template <>
inline Real
integration_coeff<TRI3, 1, 1>()
{
    return 1. / 12.;
}

template <>
inline Real
integration_coeff<TET4, 1, 1>()
{
    return 1. / 20.;
}

template <>
inline Real
integration_coeff<EDGE2, 0, 0>()
{
    return 1.;
}

template <>
inline Real
integration_coeff<TRI3, 0, 0>()
{
    return 1.;
}

template <>
inline Real
integration_coeff<TET4, 0, 0>()
{
    return 1.;
}

// Surface linear forms

/// Get coefficient for exact integration of a surface linear form
///
/// @tparam ELEM_TYPE Element type
/// @tparam N1 Order of the 1st shape function
/// @tparam N2 Order of the 2nd shape function
/// @return
template <ElementType ELEM_TYPE, Int N1, Int N2>
inline Real
surface_integration_coeff()
{
    error("Surface integration coefficient for '{}' is not implemented",
          get_element_type_str(ELEM_TYPE));
}

template <>
inline Real
surface_integration_coeff<EDGE2, 0, 1>()
{
    return 1.;
}

template <>
inline Real
surface_integration_coeff<EDGE2, 1, 1>()
{
    return 1.;
}

template <>
inline Real
surface_integration_coeff<TRI3, 0, 1>()
{
    return 1. / 2.;
}

template <>
inline Real
surface_integration_coeff<TRI3, 1, 1>()
{
    return 1. / 6.;
}

template <>
inline Real
surface_integration_coeff<TET4, 0, 1>()
{
    return 1. / 3.;
}

template <>
inline Real
surface_integration_coeff<TET4, 1, 1>()
{
    return 1. / 12.;
}

} // namespace fe

} // namespace godzilla
