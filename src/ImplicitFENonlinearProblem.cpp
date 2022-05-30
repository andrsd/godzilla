#include "Godzilla.h"
#include "CallStack.h"
#include "ImplicitFENonlinearProblem.h"
#include "Output.h"
#include "petscts.h"

namespace godzilla {

InputParameters
ImplicitFENonlinearProblem::valid_params()
{
    InputParameters params = FENonlinearProblem::valid_params();
    params += TransientProblemInterface::valid_params();
    return params;
}

ImplicitFENonlinearProblem::ImplicitFENonlinearProblem(const InputParameters & params) :
    FENonlinearProblem(params),
    TransientProblemInterface(this, params)
{
    _F_;
    this->default_output_on = Output::ON_INITIAL | Output::ON_TIMESTEP;
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
    TransientProblemInterface::init();
    ierr = TSGetSNES(this->ts, &this->snes);
    check_petsc_error(ierr);

    FEProblemInterface::init();
}

void
ImplicitFENonlinearProblem::create()
{
    _F_;
    FENonlinearProblem::create();
    TransientProblemInterface::create();
}

void
ImplicitFENonlinearProblem::solve()
{
    _F_;
    TransientProblemInterface::solve(this->x);
}

void
ImplicitFENonlinearProblem::set_up_callbacks()
{
    _F_;
    PetscErrorCode ierr;
    DM dm = get_dm();

    ierr = DMTSSetBoundaryLocal(dm, DMPlexTSComputeBoundary, this);
    check_petsc_error(ierr);
    ierr = DMTSSetIFunctionLocal(dm, DMPlexTSComputeIFunctionFEM, this);
    check_petsc_error(ierr);
    ierr = DMTSSetIJacobianLocal(dm, DMPlexTSComputeIJacobianFEM, this);
    check_petsc_error(ierr);
}

void
ImplicitFENonlinearProblem::set_up_monitors()
{
    _F_;
    FENonlinearProblem::set_up_monitors();
    TransientProblemInterface::set_up_monitors();
}

} // namespace godzilla
