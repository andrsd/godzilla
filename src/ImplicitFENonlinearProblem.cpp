#include "Godzilla.h"
#include "CallStack.h"
#include "ImplicitFENonlinearProblem.h"
#include "petscts.h"

namespace godzilla {

PetscErrorCode
__transient_pre_step(TS ts)
{
    _F_;
    void * ctx;
    TSGetApplicationContext(ts, &ctx);
    ImplicitFENonlinearProblem * prob = static_cast<ImplicitFENonlinearProblem *>(ctx);
    return prob->on_pre_step();
}

PetscErrorCode
__transient_post_step(TS ts)
{
    _F_;
    void * ctx;
    TSGetApplicationContext(ts, &ctx);
    ImplicitFENonlinearProblem * prob = static_cast<ImplicitFENonlinearProblem *>(ctx);
    return prob->on_post_step();
}

PetscErrorCode
__transient_monitor(TS ts, PetscInt stepi, PetscReal time, Vec X, void * ctx)
{
    _F_;
    ImplicitFENonlinearProblem * prob = static_cast<ImplicitFENonlinearProblem *>(ctx);
    return prob->ts_monitor_callback(stepi, time, X);
}

InputParameters
ImplicitFENonlinearProblem::valid_params()
{
    InputParameters params = FENonlinearProblem::valid_params();
    params += TransientInterface::valid_params();
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
    ierr = TSSetApplicationContext(this->ts, this);
    checkPetscError(ierr);
    ierr = TSGetSNES(this->ts, &this->snes);
    checkPetscError(ierr);
}

void
ImplicitFENonlinearProblem::create()
{
    _F_;
    FENonlinearProblem::create();
    TransientInterface::create(get_dm());
}

PetscErrorCode
ImplicitFENonlinearProblem::on_pre_step()
{
    _F_;
    return 0;
}

PetscErrorCode
ImplicitFENonlinearProblem::on_post_step()
{
    _F_;
    PetscErrorCode ierr;
    Vec sln;
    ierr = TSGetSolution(this->ts, &sln);
    checkPetscError(ierr);
    return 0;
}

PetscErrorCode
ImplicitFENonlinearProblem::ts_monitor_callback(PetscInt stepi, PetscReal t, Vec X)
{
    _F_;
    PetscErrorCode ierr;
    PetscReal dt;
    ierr = TSGetTimeStep(this->ts, &dt);
    checkPetscError(ierr);
    this->time = t;
    godzilla_print(6, stepi, " Time ", t, ", dt = ", dt);
    return 0;
}

void
ImplicitFENonlinearProblem::solve()
{
    _F_;
    TransientInterface::solve(this->x);
}

void
ImplicitFENonlinearProblem::run()
{
    _F_;
    godzilla_print(5, "Executing...");
    solve();
}

void
ImplicitFENonlinearProblem::setup_callbacks()
{
    _F_;
    // PetscErrorCode ierr;
    // DM dm = get_dm();
    //
    // ierr = DMTSSetBoundaryLocal(dm, DMPlexTSComputeBoundary, this);
    // checkPetscError(ierr);
    // ierr = DMTSSetIFunctionLocal(dm, DMPlexTSComputeIFunctionFEM, this);
    // checkPetscError(ierr);
    // ierr = DMTSSetIJacobianLocal(dm, DMPlexTSComputeIJacobianFEM, this);
    // checkPetscError(ierr);
}

void
ImplicitFENonlinearProblem::setup_monitors()
{
    _F_;
    FENonlinearProblem::setup_monitors();

    PetscErrorCode ierr;
    ierr = TSSetPreStep(this->ts, __transient_pre_step);
    checkPetscError(ierr);
    ierr = TSSetPostStep(this->ts, __transient_post_step);
    checkPetscError(ierr);
    ierr = TSMonitorSet(this->ts, __transient_monitor, this, NULL);
    checkPetscError(ierr);
}

} // namespace godzilla
