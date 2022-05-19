#include "FENonlinearProblem.h"
#include "CallStack.h"
#include "Mesh.h"
#include "petscdm.h"

namespace godzilla {

InputParameters
FENonlinearProblem::valid_params()
{
    InputParameters params = NonlinearProblem::valid_params();
    return params;
}

FENonlinearProblem::FENonlinearProblem(const InputParameters & parameters) :
    NonlinearProblem(parameters),
    FEProblemInterface(this, parameters)
{
    _F_;
}

FENonlinearProblem::~FENonlinearProblem() {}

void
FENonlinearProblem::create()
{
    _F_;
    FEProblemInterface::create();
    NonlinearProblem::create();
}

void
FENonlinearProblem::init()
{
    _F_;
    NonlinearProblem::init();
    FEProblemInterface::init();
}

void
FENonlinearProblem::set_up_callbacks()
{
    _F_;
    PetscErrorCode ierr;
    DM dm = this->get_dm();
    ierr = DMPlexSetSNESLocalFEM(dm, this, this, this);
    check_petsc_error(ierr);
    ierr = SNESSetJacobian(this->snes, this->J, this->Jp, NULL, NULL);
    check_petsc_error(ierr);
}

void
FENonlinearProblem::set_up_initial_guess()
{
    _F_;
    FEProblemInterface::set_up_initial_guess(get_dm(), this->x);
}

PetscErrorCode
FENonlinearProblem::compute_residual_callback(Vec x, Vec f)
{
    _F_;
    return 0;
}

PetscErrorCode
FENonlinearProblem::compute_jacobian_callback(Vec x, Mat J, Mat Jp)
{
    _F_;
    return 0;
}

} // namespace godzilla
