// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "godzilla/Delegate.h"

namespace godzilla {

using FunctionDelegate = Delegate<void(Real, const Real[], Scalar[])>;

namespace internal {

/// Delegate for calling time-dependent spatial functions
struct FunctionMethodAbstract {
    virtual ~FunctionMethodAbstract() = default;
    virtual ErrorCode invoke(Int dim, Real time, const Real x[], Int nc, Scalar u[]) = 0;
};

ErrorCode
invoke_function_delegate(Int dim, Real time, const Real x[], Int nc, Scalar u[], void * ctx);

} // namespace internal
} // namespace godzilla
