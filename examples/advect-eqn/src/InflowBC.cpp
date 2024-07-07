#include "godzilla/Godzilla.h"
#include "godzilla/CallStack.h"
#include "InflowBC.h"

Parameters
InflowBC::parameters()
{
    Parameters params = NaturalRiemannBC::parameters();
    params.add_required_param<Real>("vel", "Inlet velocity");
    return params;
}

InflowBC::InflowBC(const Parameters & params) :
    NaturalRiemannBC(params),
    inlet_vel(get_param<Real>("vel")),
    components({ 0 })
{
    CALL_STACK_MSG();
}

const std::vector<Int> &
InflowBC::get_components() const
{
    CALL_STACK_MSG();
    return this->components;
}

void
InflowBC::evaluate(Real time,
                   const Real * c,
                   const Real * n,
                   const PetscScalar * xI,
                   PetscScalar * xG)
{
    CALL_STACK_MSG();
    xG[0] = this->inlet_vel;
}
