// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"

namespace godzilla::internal {

/// Delegate for calling time-dependent spatial functions
struct FunctionMethodAbstract {
    virtual ~FunctionMethodAbstract() = default;
    virtual ErrorCode invoke(Int dim, Real time, const Real x[], Int nc, Scalar u[]) = 0;
};

} // namespace godzilla::internal
