#include "godzilla/Godzilla.h"
#include "godzilla/FunctionInterface.h"
#include "AdvectionEquation.h"
#include "godzilla/CallStack.h"

using namespace godzilla;

REGISTER_OBJECT(AdvectionEquation);

Parameters
AdvectionEquation::parameters()
{
    Parameters params = ExplicitFVLinearProblem::parameters();
    return params;
}

AdvectionEquation::AdvectionEquation(const Parameters & parameters) :
    ExplicitFVLinearProblem(parameters)
{
    _F_;
}

void
AdvectionEquation::create()
{
    _F_;
    ExplicitFVLinearProblem::create();
    create_mass_matrix();
}

void
AdvectionEquation::set_up_fields()
{
    _F_;
    add_field(0, "u", 1);
}

PetscErrorCode
AdvectionEquation::compute_flux(PetscInt,
                                PetscInt,
                                const PetscReal[],
                                const PetscReal n[],
                                const PetscScalar uL[],
                                const PetscScalar uR[],
                                PetscInt,
                                const PetscScalar[],
                                PetscScalar flux[])
{
    _F_;
    PetscReal wind[] = { 0.5 };
    PetscReal wn = 0;
    wn += wind[0] * n[0];
    flux[0] = (wn > 0 ? uL[0] : uR[0]) * wn;
    return 0;
}
