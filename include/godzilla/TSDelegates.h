//  SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
//  SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"

namespace godzilla::internal {

// Machinery for TSMonitor

/// Abstract "method" for calling TS Monitor
struct TSMonitorMethodAbstract {
    virtual ~TSMonitorMethodAbstract() = default;
    virtual PetscErrorCode invoke(Int it, Real rnorm, const Vector & x) = 0;
};

template <typename T>
struct TSMonitorMethod : public TSMonitorMethodAbstract {
    TSMonitorMethod(T * instance, PetscErrorCode (T::*method)(Int, Real, const Vector &)) :
        instance(instance),
        method(method)
    {
    }

    PetscErrorCode
    invoke(Int it, Real rnorm, const Vector & x) override
    {
        return ((*this->instance).*method)(it, rnorm, x);
    }

private:
    T * instance;
    PetscErrorCode (T::*method)(Int, Real, const Vector &);
};

} // namespace godzilla::internal
