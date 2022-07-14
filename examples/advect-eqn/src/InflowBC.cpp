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
    inlet_vel(get_param<PetscReal>("vel"))
{
    _F_;
}

PetscInt
InflowBC::get_num_components() const
{
    _F_;
    return 1;
}

std::vector<PetscInt>
InflowBC::get_components() const
{
    _F_;
    return { 0 };
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