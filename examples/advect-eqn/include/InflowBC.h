#pragma once

#include "NaturalRiemannBC.h"

using namespace godzilla;

/// Inflow boundary condition
///
class InflowBC : public NaturalRiemannBC {
public:
    InflowBC(const Parameters & params);

    virtual PetscInt get_num_components() const override;
    virtual std::vector<PetscInt> get_components() const override;
    virtual void evaluate(PetscReal time,
                          const PetscReal * c,
                          const PetscReal * n,
                          const PetscScalar * xI,
                          PetscScalar * xG) override;

protected:
    /// Inlet velocity
    const PetscReal & inlet_vel;

public:
    static Parameters valid_params();
};
