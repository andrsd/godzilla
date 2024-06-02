// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"

namespace godzilla::internal {

// Machinery for FVComputeFlux

/// Abstract "method" for calling FVComputeFlux
struct FVComputeFluxMethodAbstract {
    virtual ~FVComputeFluxMethodAbstract() = default;
    virtual PetscErrorCode invoke(Int dim,
                                  Int nf,
                                  const Real x[],
                                  const Real n[],
                                  const Scalar uL[],
                                  const Scalar uR[],
                                  Int n_consts,
                                  const Scalar constants[],
                                  Scalar flux[]) = 0;
};

template <typename T>
struct FVComputeFluxMethod : public FVComputeFluxMethodAbstract {
    FVComputeFluxMethod(T * instance,
                        PetscErrorCode (T::*method)(Int,
                                                    Int,
                                                    const Real[],
                                                    const Real[],
                                                    const Scalar[],
                                                    const Scalar[],
                                                    Int,
                                                    const Scalar[],
                                                    Scalar[])) :
        instance(instance),
        method(method)
    {
    }

    PetscErrorCode
    invoke(Int dim,
           Int nf,
           const Real x[],
           const Real n[],
           const Scalar uL[],
           const Scalar uR[],
           Int n_consts,
           const Scalar constants[],
           Scalar flux[]) override
    {
        return ((*this->instance).*method)(dim, nf, x, n, uL, uR, n_consts, constants, flux);
    }

private:
    T * instance;
    PetscErrorCode (T::*method)(Int,
                                Int,
                                const Real[],
                                const Real[],
                                const Scalar[],
                                const Scalar[],
                                Int,
                                const Scalar[],
                                Scalar[]);
};

} // namespace godzilla::internal
