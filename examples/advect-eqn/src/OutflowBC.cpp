#include "godzilla/Godzilla.h"
#include "godzilla/CallStack.h"
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
    CALL_STACK_MSG();
}

const std::vector<PetscInt> &
OutflowBC::get_components() const
{
    CALL_STACK_MSG();
    return this->components;
}

void
OutflowBC::evaluate(PetscReal time,
                    const PetscReal * c,
                    const PetscReal * n,
                    const PetscScalar * xI,
                    PetscScalar * xG)
{
    CALL_STACK_MSG();
    xG[0] = xI[0];
}
