#pragma once

#include "NaturalRiemannBC.h"

using namespace godzilla;

/// Outflow boundary condition
///
class OutflowBC : public NaturalRiemannBC {
public:
    OutflowBC(const Parameters & params);

    const std::vector<PetscInt> & get_components() const override;
    void evaluate(PetscReal time,
                  const PetscReal * c,
                  const PetscReal * n,
                  const PetscScalar * xI,
                  PetscScalar * xG) override;

protected:
    std::vector<PetscInt> components;

public:
    static Parameters parameters();
};
