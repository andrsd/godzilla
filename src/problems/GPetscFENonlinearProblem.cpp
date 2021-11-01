#include "problems/GPetscFENonlinearProblem.h"
#include "base/CallStack.h"
#include "grids/GGrid.h"

#include "petscdm.h"
#include "petscdmlabel.h"
#include "petscds.h"
#include "petscviewerhdf5.h"

registerMooseObject("GodzillaApp", GPetscFENonlinearProblem);


InputParameters
GPetscFENonlinearProblem::validParams()
{
    InputParameters params = GPetscNonlinearProblem::validParams();
    return params;
}

GPetscFENonlinearProblem::GPetscFENonlinearProblem(const InputParameters & parameters) :
    GPetscNonlinearProblem(parameters)
{
    _F_;
}

GPetscFENonlinearProblem::~GPetscFENonlinearProblem()
{
    _F_;
}

void
GPetscFENonlinearProblem::setupProblem()
{
    _F_;
    PetscErrorCode ierr;
}

void
GPetscFENonlinearProblem::setupCallbacks()
{
    _F_;
    PetscErrorCode ierr;
    const DM & dm = this->getDM();
    ierr = DMPlexSetSNESLocalFEM(dm, this, this, this);
    ierr = SNESSetJacobian(this->snes, this->J, this->Jp, NULL, NULL);
}

void
GPetscFENonlinearProblem::setInitialGuess()
{
    _F_;
}

PetscErrorCode
GPetscFENonlinearProblem::computeResidualCallback(Vec x, Vec f)
{
    return 0;
}

PetscErrorCode
GPetscFENonlinearProblem::computeJacobianCallback(Vec x, Mat J, Mat Jp)
{
    return 0;
}

void
GPetscFENonlinearProblem::out()
{
    _F_;
}
