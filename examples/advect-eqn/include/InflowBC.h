#pragma once

#include "godzilla/NaturalRiemannBC.h"

using namespace godzilla;

/// Inflow boundary condition
///
class InflowBC : public NaturalRiemannBC {
public:
    InflowBC(const Parameters & params);

    const std::vector<Int> & get_components() const override;
    void evaluate(PetscReal time,
                  const PetscReal * c,
                  const PetscReal * n,
                  const PetscScalar * xI,
                  PetscScalar * xG) override;

protected:
    /// Inlet velocity
    const PetscReal & inlet_vel;
    std::vector<Int> components;

public:
    static Parameters parameters();
};
