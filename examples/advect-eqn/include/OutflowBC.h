#pragma once

#include "NaturalRiemannBC.h"

using namespace godzilla;

/// Outflow boundary condition
///
class OutflowBC : public NaturalRiemannBC {
public:
    OutflowBC(const InputParameters & params);

    virtual PetscInt get_num_components() const override;
    virtual std::vector<PetscInt> get_components() const override;
    virtual void evaluate(PetscReal time,
                          const PetscReal * c,
                          const PetscReal * n,
                          const PetscScalar * xI,
                          PetscScalar * xG) override;

public:
    static InputParameters valid_params();
};
