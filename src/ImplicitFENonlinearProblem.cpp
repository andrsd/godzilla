#include "Godzilla.h"
#include "CallStack.h"
#include "ImplicitFENonlinearProblem.h"
#include "Output.h"
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
__transient_monitor(TS ts, PetscInt stepi, PetscReal time, Vec x, void * ctx)
{
    _F_;
    ImplicitFENonlinearProblem * prob = static_cast<ImplicitFENonlinearProblem *>(ctx);
    return prob->ts_monitor_callback(stepi, time, x);
}

InputParameters
ImplicitFENonlinearProblem::valid_params()
{
    InputParameters params = FENonlinearProblem::valid_params();
    params += TransientProblemInterface::valid_params();
    return params;
}

ImplicitFENonlinearProblem::ImplicitFENonlinearProblem(const InputParameters & params) :
    FENonlinearProblem(params),
    TransientProblemInterface(params)
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
    TransientProblemInterface::init(get_comm());
    ierr = TSSetApplicationContext(this->ts, this);
    check_petsc_error(ierr);
    ierr = TSGetSNES(this->ts, &this->snes);
    check_petsc_error(ierr);

    FEProblemInterface::init();
}

void
ImplicitFENonlinearProblem::create()
{
    _F_;
    FENonlinearProblem::create();
    TransientProblemInterface::create(get_dm());
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

    ierr = TSGetTime(this->ts, &this->time);
    check_petsc_error(ierr);
    ierr = TSGetStepNumber(this->ts, &this->step_num);
    check_petsc_error(ierr);
    Vec sln;
    ierr = TSGetSolution(this->ts, &sln);
    check_petsc_error(ierr);
    ierr = VecCopy(sln, this->x);
    check_petsc_error(ierr);
    compute_postprocessors();
    output();
    return 0;
}

PetscErrorCode
ImplicitFENonlinearProblem::ts_monitor_callback(PetscInt stepi, PetscReal t, Vec x)
{
    _F_;
    PetscErrorCode ierr;
    PetscReal dt;
    ierr = TSGetTimeStep(this->ts, &dt);
    check_petsc_error(ierr);
    godzilla_print(6, "%d Time %g dt = %g", stepi, t, dt);
    return 0;
}

void
ImplicitFENonlinearProblem::solve()
{
    _F_;
    TransientProblemInterface::solve(this->x);
}

void
ImplicitFENonlinearProblem::run()
{
    _F_;
    godzilla_print(5, "Executing...");
    // output initial condition
    output();
    solve();
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

    PetscErrorCode ierr;
    ierr = TSSetPreStep(this->ts, __transient_pre_step);
    check_petsc_error(ierr);
    ierr = TSSetPostStep(this->ts, __transient_post_step);
    check_petsc_error(ierr);
    ierr = TSMonitorSet(this->ts, __transient_monitor, this, NULL);
    check_petsc_error(ierr);
}

void
ImplicitFENonlinearProblem::output()
{
    _F_;
    for (auto & o : this->outputs)
        o->output_step(this->step_num);
}

} // namespace godzilla
