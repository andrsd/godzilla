#include "godzilla/Godzilla.h"
#include "godzilla/CallStack.h"
#include "InflowBC.h"

Parameters
InflowBC::parameters()
{
    Parameters params = NaturalRiemannBC::parameters();
    params.add_required_param<PetscReal>("vel", "Inlet velocity");
    return params;
}

InflowBC::InflowBC(const Parameters & params) :
    NaturalRiemannBC(params),
    inlet_vel(get_param<PetscReal>("vel")),
    components({ 0 })
{
    CALL_STACK_MSG();
}

const std::vector<PetscInt> &
InflowBC::get_components() const
{
    CALL_STACK_MSG();
    return this->components;
}

void
InflowBC::evaluate(PetscReal time,
                   const PetscReal * c,
                   const PetscReal * n,
                   const PetscScalar * xI,
                   PetscScalar * xG)
{
    CALL_STACK_MSG();
    xG[0] = this->inlet_vel;
}
