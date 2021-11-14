#include "Godzilla.h"
#include "CallStack.h"
#include "ImplicitFENonlinearProblem.h"
#include "petscts.h"

namespace godzilla {

InputParameters
ImplicitFENonlinearProblem::validParams()
{
    InputParameters params = FENonlinearProblem::validParams();
    params += TransientInterface::validParams();
    return params;
}

ImplicitFENonlinearProblem::ImplicitFENonlinearProblem(const InputParameters & params) :
    FENonlinearProblem(params),
    TransientInterface(params)
{
    _F_;
}

ImplicitFENonlinearProblem::~ImplicitFENonlinearProblem()
{
    this->snes = nullptr;
}

void
ImplicitFENonlinearProblem::init()
{
    _F_;
    PetscErrorCode ierr;
    TransientInterface::init(comm());
    ierr = TSGetSNES(this->ts, &this->snes);
    checkPetscError(ierr);
    FEProblemInterface::init(comm(), getDM());
}

void
ImplicitFENonlinearProblem::create()
{
    _F_;
    FENonlinearProblem::create();
    TransientInterface::create(getDM());
}

void
ImplicitFENonlinearProblem::solve()
{
    _F_;
    godzillaPrint(5, "Executing...");
    TransientInterface::solve(this->x);
}

void
ImplicitFENonlinearProblem::setupCallbacks()
{
    _F_;
    PetscErrorCode ierr;
    DM dm = getDM();

    ierr = DMTSSetBoundaryLocal(dm, DMPlexTSComputeBoundary, this);
    checkPetscError(ierr);
    ierr = DMTSSetIFunctionLocal(dm, DMPlexTSComputeIFunctionFEM, this);
    checkPetscError(ierr);
    ierr = DMTSSetIJacobianLocal(dm, DMPlexTSComputeIJacobianFEM, this);
    checkPetscError(ierr);
}

} // namespace godzilla
