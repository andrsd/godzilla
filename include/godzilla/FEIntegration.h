// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "godzilla/Error.h"
#include "godzilla/Exception.h"

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
    throw NotImplementedException("Integration coefficient for '{}' is not implemented",
                                  get_element_type_str(ELEM_TYPE));
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
integration_coeff<TRI3, 2>()
{
    return 1. / 6.;
}

template <>
inline Real
integration_coeff<TRI3, 3>()
{
    return 1. / 10.;
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
    throw NotImplementedException("Integration coefficient for '{}' is not implemented",
                                  get_element_type_str(ELEM_TYPE));
}

template <>
inline Real
integration_coeff<EDGE2, 1, 1>()
{
    return 1. / 6.;
}

template <>
inline Real
integration_coeff<EDGE2, 1, 2>()
{
    return 1. / 12.;
}

template <>
inline Real
integration_coeff<EDGE2, 2, 1>()
{
    return integration_coeff<EDGE2, 1, 2>();
}

template <>
inline Real
integration_coeff<TRI3, 1, 1>()
{
    return 1. / 12.;
}

template <>
inline Real
integration_coeff<TRI3, 1, 2>()
{
    return 1. / 30.;
}

template <>
inline Real
integration_coeff<TRI3, 1, 3>()
{
    return 1. / 60.;
}

template <>
inline Real
integration_coeff<TRI3, 2, 1>()
{
    return integration_coeff<TRI3, 1, 2>();
}

template <>
inline Real
integration_coeff<TRI3, 3, 1>()
{
    return integration_coeff<TRI3, 1, 3>();
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
    throw NotImplementedException("Surface integration coefficient for '{}' is not implemented",
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
