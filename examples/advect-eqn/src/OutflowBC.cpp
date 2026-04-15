#include "godzilla/CallStack.h"
#include "OutflowBC.h"

Parameters
OutflowBC::parameters()
{
    Parameters params = NaturalRiemannBC::parameters();
    return params;
}

OutflowBC::OutflowBC(const Parameters & pars) : NaturalRiemannBC(pars)
{
    CALL_STACK_MSG();
}

void
OutflowBC::evaluate(Real, const Real *, const Real *, const Scalar * xI, Scalar * xG)
{
    CALL_STACK_MSG();
    xG[0] = xI[0];
}
