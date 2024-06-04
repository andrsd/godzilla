// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"

namespace godzilla::internal {

// Machinery for calling time-dependent spatial functions

/// Abstract "method"
struct FunctionMethodAbstract {
    virtual ~FunctionMethodAbstract() = default;
    virtual ErrorCode invoke(Int dim, Real time, const Real x[], Int nc, Scalar u[]) = 0;
};

template <typename T>
struct ICFunctionMethod : public FunctionMethodAbstract {
    ICFunctionMethod(T * instance, void (T::*method)(Real, const Real[], Scalar[])) :
        instance(instance),
        method(method)
    {
    }

    ErrorCode
    invoke(Int dim, Real time, const Real x[], Int nc, Scalar u[]) override
    {
        ((*this->instance).*method)(time, x, u);
        return 0;
    }

private:
    T * instance;
    void (T::*method)(Real, const Real[], Scalar[]);
};

} // namespace godzilla::internal
