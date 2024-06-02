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

// Machinery for TSComputeRhsFunction

/// Abstract "method" for calling TSComputeRhsMethod
struct TSComputeRhsMethodAbstract {
    virtual ~TSComputeRhsMethodAbstract() = default;
    virtual PetscErrorCode invoke(Real time, const Vector & x, Vector & F) = 0;
};

template <typename T>
struct TSComputeRhsMethod : public TSComputeRhsMethodAbstract {
    TSComputeRhsMethod(T * instance, PetscErrorCode (T::*method)(Real, const Vector &, Vector &)) :
        instance(instance),
        method(method)
    {
    }

    PetscErrorCode
    invoke(Real time, const Vector & x, Vector & F) override
    {
        return ((*this->instance).*method)(time, x, F);
    }

private:
    T * instance;
    PetscErrorCode (T::*method)(Real, const Vector &, Vector &);
};

} // namespace godzilla::internal
