#include "godzilla/CallStack.h"
#include "InflowBC.h"

Parameters
InflowBC::parameters()
{
    auto params = NaturalRiemannBC::parameters();
    params.add_required_param<Real>("vel", "Inlet velocity");
    return params;
}

InflowBC::InflowBC(const Parameters & pars) :
    NaturalRiemannBC(pars),
    inlet_vel(pars.get<Real>("vel"))
{
    CALL_STACK_MSG();
}

void
InflowBC::evaluate(Real time, const Real * c, const Real * n, const Scalar * xI, Scalar * xG)
{
    CALL_STACK_MSG();
    xG[0] = this->inlet_vel;
}
