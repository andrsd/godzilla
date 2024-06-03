// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "petscksp.h"
#include "godzilla/Types.h"
#include "godzilla/Error.h"
#include "godzilla/Vector.h"
#include "godzilla/Matrix.h"

namespace godzilla::internal {

// Machinery for KSPMonitor

/// Abstract "method" for calling KSP Monitor
struct KSPMonitorMethodAbstract {
    virtual ~KSPMonitorMethodAbstract() = default;
    virtual ErrorCode invoke(Int it, Real rnorm) = 0;
};

template <typename T>
struct KSPMonitorMethod : public KSPMonitorMethodAbstract {
    KSPMonitorMethod(T * instance, ErrorCode (T::*monitor)(Int, Real)) :
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

// Machinery for KSPComputeRhs

/// Abstract "method" for calling KSP compute rhs
struct KSPComputeRhsMethodAbstract {
    virtual ErrorCode invoke(Vector & b) = 0;
};

template <typename T>
struct KSPComputeRhsMethod : public KSPComputeRhsMethodAbstract {
    KSPComputeRhsMethod(T * instance, ErrorCode (T::*method)(Vector & b)) :
        instance(instance),
        method(method)
    {
    }

    ErrorCode
    invoke(Vector & b) override
    {
        return ((*this->instance).*method)(b);
    }

private:
    T * instance;
    ErrorCode (T::*method)(Vector &);
};

// Machinery for KSPComputeOperators

/// Abstract "method" for calling KSP compute operators
struct KSPComputeOperatorsMethodAbstract {
    virtual ErrorCode invoke(Matrix & A, Matrix & B) = 0;
};

template <typename T>
struct KSPComputeOperatorsMethod : public KSPComputeOperatorsMethodAbstract {
    KSPComputeOperatorsMethod(T * instance, ErrorCode (T::*method)(Matrix &, Matrix &)) :
        instance(instance),
        method(method)
    {
    }

    ErrorCode
    invoke(Matrix & A, Matrix & B) override
    {
        return ((*this->instance).*method)(A, B);
    }

private:
    T * instance;
    ErrorCode (T::*method)(Matrix &, Matrix &);
};

} // namespace godzilla::internal
