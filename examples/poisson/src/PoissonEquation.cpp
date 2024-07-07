#include "godzilla/Godzilla.h"
#include "godzilla/FunctionInterface.h"
#include "godzilla/WeakForm.h"
#include "PoissonEquation.h"
#include "PoissonPDE.h"
#include "godzilla/CallStack.h"

using namespace godzilla;

Parameters
PoissonEquation::parameters()
{
    Parameters params = FENonlinearProblem::parameters();
    params.add_param<Int>("p_order", 1., "Polynomial order of the FE space.");
    return params;
}

PoissonEquation::PoissonEquation(const Parameters & parameters) :
    FENonlinearProblem(parameters),
    p_order(get_param<Int>("p_order"))
{
    CALL_STACK_MSG();
}

void
PoissonEquation::set_up_fields()
{
    CALL_STACK_MSG();
    this->iu = add_field("u", 1, this->p_order);
    this->affn = add_aux_field("forcing_fn", 1, this->p_order);
}

void
PoissonEquation::set_up_weak_form()
{
    CALL_STACK_MSG();
    add_residual_block<WeakForm::F0>(this->iu, new Residual0(this));
    add_residual_block<WeakForm::F1>(this->iu, new Residual1(this));

    add_jacobian_block<WeakForm::G3>(this->iu, this->iu, new Jacobian3(this));
}
