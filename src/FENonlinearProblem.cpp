#include "FENonlinearProblem.h"
#include "CallStack.h"
#include "Grid.h"
#include "petscdm.h"

namespace godzilla {

InputParameters
FENonlinearProblem::validParams()
{
    InputParameters params = NonlinearProblem::validParams();
    return params;
}

FENonlinearProblem::FENonlinearProblem(const InputParameters & parameters) :
    NonlinearProblem(parameters),
    FEProblemInterface(parameters)
{
    _F_;
}

FENonlinearProblem::~FENonlinearProblem() {}

void
FENonlinearProblem::create()
{
    _F_;
    FEProblemInterface::create(getDM());
    NonlinearProblem::create();
}

void
FENonlinearProblem::init()
{
    _F_;
    NonlinearProblem::init();
    FEProblemInterface::init(getDM());
}

void
FENonlinearProblem::setUpCallbacks()
{
    _F_;
    PetscErrorCode ierr;
    DM dm = this->getDM();
    ierr = DMPlexSetSNESLocalFEM(dm, this, this, this);
    checkPetscError(ierr);
    ierr = SNESSetJacobian(this->snes, this->J, this->Jp, NULL, NULL);
    checkPetscError(ierr);
}

void
FENonlinearProblem::setUpInitialGuess()
{
    _F_;
    FEProblemInterface::setUpInitialGuess(getDM(), this->x);
}

PetscErrorCode
FENonlinearProblem::computeResidualCallback(Vec x, Vec f)
{
    _F_;
    return 0;
}

PetscErrorCode
FENonlinearProblem::computeJacobianCallback(Vec x, Mat J, Mat Jp)
{
    _F_;
    return 0;
}

} // namespace godzilla
