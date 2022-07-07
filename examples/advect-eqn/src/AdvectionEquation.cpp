#include "Godzilla.h"
#include "FunctionInterface.h"
#include "AdvectionEquation.h"
#include "CallStack.h"

using namespace godzilla;

REGISTER_OBJECT(AdvectionEquation);

///

Parameters
AdvectionEquation::valid_params()
{
    Parameters params = ExplicitFVLinearProblem::valid_params();
    return params;
}

AdvectionEquation::AdvectionEquation(const Parameters & parameters) :
    ExplicitFVLinearProblem(parameters)
{
    _F_;
}

void
AdvectionEquation::set_up_fields()
{
    _F_;
    add_field(0, "u", 1);
}

void
AdvectionEquation::compute_flux(PetscInt dim,
                                PetscInt nf,
                                const PetscReal x[],
                                const PetscReal n[],
                                const PetscScalar uL[],
                                const PetscScalar uR[],
                                PetscInt n_consts,
                                const PetscScalar constants[],
                                PetscScalar flux[])
{
    _F_;
    PetscReal wind[] = { 0.5 };
    PetscReal wn = 0;
    wn += wind[0] * n[0];
    flux[0] = (wn > 0 ? uL[0] : uR[0]) * wn;
}
