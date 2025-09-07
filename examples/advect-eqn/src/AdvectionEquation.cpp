#include "godzilla/FunctionInterface.h"
#include "AdvectionEquation.h"
#include "godzilla/CallStack.h"
#include "godzilla/Types.h"

using namespace godzilla;

Parameters
AdvectionEquation::parameters()
{
    auto params = ExplicitFVLinearProblem::parameters();
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
    add_field(FieldID(0), "u", 1);
}

void
AdvectionEquation::set_up_weak_form()
{
    CALL_STACK_MSG();
    set_riemann_solver(FieldID(0), this, &AdvectionEquation::compute_flux);
}

void
AdvectionEquation::compute_flux(const Real[],
                                const Real n[],
                                const Scalar u_l[],
                                const Scalar u_r[],
                                Scalar flux[])
{
    CALL_STACK_MSG();
    Real wind[] = { 0.5 };
    Real wn = 0;
    wn += wind[0] * n[0];
    flux[0] = (wn > 0 ? u_l[0] : u_r[0]) * wn;
}
