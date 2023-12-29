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
    CALL_STACK_MSG();
}

void
AdvectionEquation::create()
{
    CALL_STACK_MSG();
    ExplicitFVLinearProblem::create();
    create_mass_matrix();
}

void
AdvectionEquation::set_up_fields()
{
    CALL_STACK_MSG();
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
    CALL_STACK_MSG();
    PetscReal wind[] = { 0.5 };
    PetscReal wn = 0;
    wn += wind[0] * n[0];
    flux[0] = (wn > 0 ? uL[0] : uR[0]) * wn;
    return 0;
}
