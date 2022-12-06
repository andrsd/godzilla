#include "Godzilla.h"
#include "CallStack.h"
#include "OutflowBC.h"

REGISTER_OBJECT(OutflowBC);

Parameters
OutflowBC::parameters()
{
    Parameters params = NaturalRiemannBC::parameters();
    return params;
}

OutflowBC::OutflowBC(const Parameters & params) : NaturalRiemannBC(params), components({ 0 })
{
    _F_;
}

const std::vector<PetscInt> &
OutflowBC::get_components() const
{
    _F_;
    return this->components;
}

void
OutflowBC::evaluate(PetscReal time,
                    const PetscReal * c,
                    const PetscReal * n,
                    const PetscScalar * xI,
                    PetscScalar * xG)
{
    _F_;
    xG[0] = xI[0];
}
