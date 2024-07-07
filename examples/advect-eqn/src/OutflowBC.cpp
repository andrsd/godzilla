#include "godzilla/Godzilla.h"
#include "godzilla/CallStack.h"
#include "OutflowBC.h"

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

const std::vector<Int> &
OutflowBC::get_components() const
{
    CALL_STACK_MSG();
    return this->components;
}

void
OutflowBC::evaluate(Real time,
                    const Real * c,
                    const Real * n,
                    const PetscScalar * xI,
                    PetscScalar * xG)
{
    CALL_STACK_MSG();
    xG[0] = xI[0];
}
