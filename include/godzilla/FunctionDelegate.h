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

template <typename T>
struct AuxFunctionMethod : public FunctionMethodAbstract {
    AuxFunctionMethod(T * instance, void (T::*method)(Real, const Real[], Scalar[])) :
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

/// Abstract "method" for calling EssentialBC functions
struct EssentialBCFunctionMethodAbstract : public FunctionMethodAbstract {
    virtual ErrorCode invoke_t(Int dim, Real time, const Real x[], Int nc, Scalar u[]) = 0;
};

template <typename T>
struct EssentialBCFunctionMethod : public EssentialBCFunctionMethodAbstract {
    EssentialBCFunctionMethod(T * instance,
                              void (T::*method)(Real, const Real[], Scalar[]),
                              void (T::*method_t)(Real, const Real[], Scalar[])) :
        instance(instance),
        method(method),
        method_t(method_t)
    {
    }

    ErrorCode
    invoke(Int dim, Real time, const Real x[], Int nc, Scalar u[]) override
    {
        ((*this->instance).*method)(time, x, u);
        return 0;
    }

    ErrorCode
    invoke_t(Int dim, Real time, const Real x[], Int nc, Scalar u[]) override
    {
        ((*this->instance).*method_t)(time, x, u);
        return 0;
    }

private:
    T * instance;
    void (T::*method)(Real, const Real[], Scalar[]);
    void (T::*method_t)(Real, const Real[], Scalar[]);
};

// Machinery for NaturalRiemannBC function

struct NaturalRiemannBCFunctionMethodAbstract {
    virtual ~NaturalRiemannBCFunctionMethodAbstract() = default;
    virtual ErrorCode
    invoke(Real time, const Real * c, const Real * n, const Scalar * xI, Scalar * xG) = 0;
};

template <typename T>
struct NaturalRiemannBCFunctionMethod : public NaturalRiemannBCFunctionMethodAbstract {
    NaturalRiemannBCFunctionMethod(
        T * instance,
        void (T::*method)(Real, const Real *, const Real *, const Scalar *, Scalar *)) :
        instance(instance),
        method(method)
    {
    }

    ErrorCode
    invoke(Real time, const Real * c, const Real * n, const Scalar * xI, Scalar * xG) override
    {
        ((*this->instance).*method)(time, c, n, xI, xG);
        return 0;
    }

private:
    T * instance;
    void (T::*method)(Real, const Real *, const Real *, const Scalar *, Scalar *);
};

} // namespace godzilla::internal
