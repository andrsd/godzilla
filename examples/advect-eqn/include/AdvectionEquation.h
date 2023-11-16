#pragma once

#include "godzilla/ExplicitFVLinearProblem.h"

using namespace godzilla;

/// Solves advection equation using finite volume method
///
class AdvectionEquation : public ExplicitFVLinearProblem {
public:
    AdvectionEquation(const Parameters & parameters);
    void create() override;

    void compute_flux(PetscInt dim,
                      PetscInt nf,
                      const PetscReal x[],
                      const PetscReal n[],
                      const PetscScalar uL[],
                      const PetscScalar uR[],
                      PetscInt n_consts,
                      const PetscScalar constants[],
                      PetscScalar flux[]);

protected:
    void set_up_fields() override;
    void set_up_ds() override;

public:
    static Parameters parameters();
};
