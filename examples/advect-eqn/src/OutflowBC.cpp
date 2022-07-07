#include "Godzilla.h"
#include "CallStack.h"
#include "OutflowBC.h"

REGISTER_OBJECT(OutflowBC);

Parameters
OutflowBC::valid_params()
{
    Parameters params = NaturalRiemannBC::valid_params();
    return params;
}

OutflowBC::OutflowBC(const Parameters & params) : NaturalRiemannBC(params)
{
    _F_;
}

PetscInt
OutflowBC::get_num_components() const
{
    _F_;
    return 1;
}

std::vector<PetscInt>
OutflowBC::get_components() const
{
    _F_;
    return { 0 };
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
