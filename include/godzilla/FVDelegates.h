// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"

namespace godzilla::internal {

// Machinery for FVComputeFlux

/// Abstract "method" for calling FVComputeFlux
struct FVComputeFluxMethodAbstract {
    virtual ~FVComputeFluxMethodAbstract() = default;
    virtual PetscErrorCode invoke(const Real x[],
                                  const Real n[],
                                  const Scalar u_l[],
                                  const Scalar u_r[],
                                  Scalar flux[]) = 0;
};

template <typename T>
struct FVComputeFluxMethod : public FVComputeFluxMethodAbstract {
    FVComputeFluxMethod(T * instance,
                        PetscErrorCode (T::*method)(const Real[],
                                                    const Real[],
                                                    const Scalar[],
                                                    const Scalar[],
                                                    Scalar[])) :
        instance(instance),
        method(method)
    {
    }

    PetscErrorCode
    invoke(const Real x[], const Real n[], const Scalar u_l[], const Scalar u_r[], Scalar flux[]) override
    {
        return ((*this->instance).*method)(x, n, u_l, u_r, flux);
    }

private:
    T * instance;
    PetscErrorCode (
        T::*method)(const Real[], const Real[],
                                const Scalar[],
                                const Scalar[],
                                Scalar[]);
};

} // namespace godzilla::internal
