#include "Godzilla.h"
#include "CallStack.h"
#include "InflowBC.h"

REGISTER_OBJECT(InflowBC);

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
    _F_;
}

const std::vector<PetscInt> &
InflowBC::get_components() const
{
    _F_;
    return this->components;
}

void
InflowBC::evaluate(PetscReal time,
                   const PetscReal * c,
                   const PetscReal * n,
                   const PetscScalar * xI,
                   PetscScalar * xG)
{
    _F_;
    xG[0] = this->inlet_vel;
}
