// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "godzilla/Vector.h"
#include "godzilla/Matrix.h"

namespace godzilla::internal {

// Machinery for SNESMonitor

/// Abstract "method" for calling SNES Monitor
struct SNESMonitorMethodAbstract {
    virtual ~SNESMonitorMethodAbstract() = default;
    virtual ErrorCode invoke(Int it, Real rnorm) = 0;
};

template <typename T>
struct SNESMonitorMethod : public SNESMonitorMethodAbstract {
    SNESMonitorMethod(T * instance, ErrorCode (T::*monitor)(Int, Real)) :
        instance(instance),
        monitor(monitor)
    {
    }

    ErrorCode
    invoke(Int it, Real rnorm) override
    {
        return ((*this->instance).*monitor)(it, rnorm);
    }

private:
    T * instance;
    ErrorCode (T::*monitor)(Int, Real);
};

// Machinery for SNESComputeResidual

/// Abstract "method" for calling SNES compute residual
struct SNESComputeResidualMethodAbstract {
    virtual ErrorCode invoke(const Vector & x, Vector & f) = 0;
};

template <typename T>
struct SNESComputeResidualMethod : public SNESComputeResidualMethodAbstract {
    SNESComputeResidualMethod(T * instance, ErrorCode (T::*method)(const Vector &, Vector &)) :
        instance(instance),
        method(method)
    {
    }

    ErrorCode
    invoke(const Vector & x, Vector & f) override
    {
        return ((*this->instance).*method)(x, f);
    }

private:
    T * instance;
    ErrorCode (T::*method)(const Vector & x, Vector & f);
};

// Machinery for SNESComputeJacobian

/// Abstract "method" for calling SNES compute jacobian
struct SNESComputeJacobianMethodAbstract {
    virtual ErrorCode invoke(const Vector & x, Matrix & J, Matrix & Jp) = 0;
};

template <typename T>
struct SNESComputeJacobianMethod : public SNESComputeJacobianMethodAbstract {
    SNESComputeJacobianMethod(T * instance,
                              ErrorCode (T::*method)(const Vector &, Matrix &, Matrix &)) :
        instance(instance),
        method(method)
    {
    }

    ErrorCode
    invoke(const Vector & x, Matrix & J, Matrix & Jp) override
    {
        return ((*this->instance).*method)(x, J, Jp);
    }

private:
    T * instance;
    ErrorCode (T::*method)(const Vector &, Matrix &, Matrix &);
};

} // namespace godzilla::internal
