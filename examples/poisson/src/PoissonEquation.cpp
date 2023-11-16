#include "godzilla/Godzilla.h"
#include "godzilla/FunctionInterface.h"
#include "godzilla/WeakForm.h"
#include "PoissonEquation.h"
#include "PoissonPDE.h"
#include "godzilla/CallStack.h"
#include "petscdmlabel.h"

using namespace godzilla;

REGISTER_OBJECT(PoissonEquation);

Parameters
PoissonEquation::parameters()
{
    Parameters params = FENonlinearProblem::parameters();
    params.add_param<PetscInt>("p_order", 1., "Polynomial order of the FE space.");
    return params;
}

PoissonEquation::PoissonEquation(const Parameters & parameters) :
    FENonlinearProblem(parameters),
    p_order(get_param<PetscInt>("p_order"))
{
    _F_;
}

PoissonEquation::~PoissonEquation() {}

void
PoissonEquation::set_up_fields()
{
    _F_;
    this->iu = add_fe("u", 1, this->p_order);
    this->affn = add_aux_fe("forcing_fn", 1, this->p_order);
}

void
PoissonEquation::set_up_weak_form()
{
    _F_;
    add_residual_block(this->iu, new Residual0(this), new Residual1(this));
    add_jacobian_block(this->iu, this->iu, nullptr, nullptr, nullptr, new Jacobian3(this));
}
