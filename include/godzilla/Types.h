// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Enums.h"
#include "godzilla/Flags.h"
#include "godzilla/Delegate.h"
#include "petscsystypes.h"

namespace godzilla {

using Int = PetscInt;
using Real = PetscReal;
using Scalar = PetscScalar;
using ErrorCode = PetscErrorCode;

typedef ErrorCode PetscFunc(Int dim, Real time, const Real x[], Int Nc, Scalar u[], void * ctx);

using FunctionDelegate = Delegate<void(Real, const Real[], Scalar[])>;

namespace internal {

ErrorCode
invoke_function_delegate(Int dim, Real time, const Real x[], Int nc, Scalar u[], void * ctx);

} // namespace internal

//

/// Return number of nodes given FE type
///
/// @param type Element type
/// @return Number of nodes per element
constexpr Int
get_num_element_nodes(ElementType type)
{
    switch (type) {
    case EDGE2:
        return 2;
    case TRI3:
        return 3;
    case QUAD4:
        return 4;
    case TET4:
        return 4;
    case HEX8:
        return 8;
    }
}

using ExecuteOn = Flags<ExecuteOnFlag>;

} // namespace godzilla
