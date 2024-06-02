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

// Machinery for compute IFunction

/// Abstract "method" for calling ComputeIFunctionMethod
struct TSComputeIFunctionMethodAbstract {
    virtual ~TSComputeIFunctionMethodAbstract() = default;
    virtual PetscErrorCode invoke(Real time, const Vector & x, const Vector & x_t, Vector & F) = 0;
};

template <typename T>
struct TSComputeIFunctionMethod : public TSComputeIFunctionMethodAbstract {
    TSComputeIFunctionMethod(
        T * instance,
        PetscErrorCode (T::*method)(Real, const Vector &, const Vector &, Vector &)) :
        instance(instance),
        method(method)
    {
    }

    PetscErrorCode
    invoke(Real time, const Vector & x, const Vector & x_t, Vector & F) override
    {
        return ((*this->instance).*method)(time, x, x_t, F);
    }

private:
    T * instance;
    PetscErrorCode (T::*method)(Real, const Vector &, const Vector &, Vector &);
};

// Machinery for compute IJacobian

/// Abstract "method" for calling ComputeIJacobianMethod
struct TSComputeIJacobianMethodAbstract {
    virtual ~TSComputeIJacobianMethodAbstract() = default;
    virtual PetscErrorCode invoke(Real time,
                                  const Vector & X,
                                  const Vector & X_t,
                                  Real x_t_shift,
                                  Matrix & J,
                                  Matrix & Jp) = 0;
};

template <typename T>
struct TSComputeIJacobianMethod : public TSComputeIJacobianMethodAbstract {
    TSComputeIJacobianMethod(T * instance,
                             PetscErrorCode (T::*method)(Real,
                                                         const Vector &,
                                                         const Vector &,
                                                         Real,
                                                         Matrix &,
                                                         Matrix &)) :
        instance(instance),
        method(method)
    {
    }

    PetscErrorCode
    invoke(Real time, const Vector & x, const Vector & x_t, Real x_t_shift, Matrix & J, Matrix & Jp)
        override
    {
        return ((*this->instance).*method)(time, x, x_t, x_t_shift, J, Jp);
    }

private:
    T * instance;
    PetscErrorCode (T::*method)(Real, const Vector &, const Vector &, Real, Matrix &, Matrix &);
};

// Machinery for compute TSComputeBoundary

/// Abstract "method" for calling TSComputeBoundary
struct TSComputeBoundaryMethodAbstract {
    virtual ~TSComputeBoundaryMethodAbstract() = default;
    virtual PetscErrorCode invoke(Real time, const Vector & x, const Vector & x_t) = 0;
};

template <typename T>
struct TSComputeBoundaryMethod : public TSComputeBoundaryMethodAbstract {
    TSComputeBoundaryMethod(T * instance,
                            PetscErrorCode (T::*method)(Real, const Vector &, const Vector &)) :
        instance(instance),
        method(method)
    {
    }

    PetscErrorCode
    invoke(Real time, const Vector & x, const Vector & x_t) override
    {
        return ((*this->instance).*method)(time, x, x_t);
    }

private:
    T * instance;
    PetscErrorCode (T::*method)(Real, const Vector &, const Vector &);
};

} // namespace godzilla::internal
