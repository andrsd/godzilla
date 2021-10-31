#include "problems/GPetscNonlinearProblem.h"
#include "base/CallStack.h"
#include "mesh/GMesh.h"
#include "petscds.h"

PetscErrorCode
__compute_jacobian(SNES snes, Vec x, Mat jac, Mat B, void *ctx)
{
    GPetscNonlinearProblem * problem = static_cast<GPetscNonlinearProblem *>(ctx);
    return problem->computeJacobianCallback(jac, x);
}


InputParameters
GPetscNonlinearProblem::validParams()
{
    InputParameters params = GProblem::validParams();
    return params;
}

GPetscNonlinearProblem::GPetscNonlinearProblem(const InputParameters & parameters) :
    GProblem(parameters)
{
    _F_;
}

GPetscNonlinearProblem::~GPetscNonlinearProblem()
{
    _F_;
    SNESDestroy(&this->snes);
    VecDestroy(&this->x);
    if (this->A != this->J)
        MatDestroy(&this->A);
    MatDestroy(&this->J);
}

void
GPetscNonlinearProblem::create()
{
    _F_;
    PetscErrorCode ierr;

    const DM & dm = getDM();
    ierr = SNESCreate(comm().get(), &this->snes);
    ierr = SNESSetDM(this->snes, dm);
    ierr = DMSetApplicationContext(dm, this);

    setupProblem();

    ierr = DMCreateGlobalVector(dm, &this->x);
    ierr = PetscObjectSetName((PetscObject) this->x, "");

    ierr = DMCreateMatrix(dm, &this->J);
    // full newton
    this->A = this->J;

    setupJacobian();
    ierr = SNESSetFromOptions(this->snes);

    setInitialGuess();
}

void
GPetscNonlinearProblem::setupJacobian()
{
    _F_;
    PetscErrorCode ierr;

    ierr = SNESSetJacobian(this->snes, this->A, this->J, __compute_jacobian, this);
}

PetscErrorCode
GPetscNonlinearProblem::computeJacobianCallback(Mat jac, Vec x)
{
    return 0;
}

void
GPetscNonlinearProblem::solve()
{
    _F_;
    PetscErrorCode ierr;

    ierr = SNESSolve(this->snes, NULL, this->x);
    ierr = SNESGetConvergedReason(this->snes, &this->converged_reason);
    ierr = SNESGetSolution(this->snes, &this->x);
}

bool
GPetscNonlinearProblem::converged()
{
    _F_;
    bool conv =
        (this->converged_reason == SNES_CONVERGED_FNORM_ABS) ||
        (this->converged_reason == SNES_CONVERGED_FNORM_RELATIVE) ||
        (this->converged_reason == SNES_CONVERGED_SNORM_RELATIVE) ||
        (this->converged_reason == SNES_CONVERGED_ITS);
    return conv;
}

void
GPetscNonlinearProblem::out()
{
}
