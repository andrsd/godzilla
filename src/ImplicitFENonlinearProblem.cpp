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
    check_petsc_error(ierr);
    ierr = TSGetSNES(this->ts, &this->snes);
    check_petsc_error(ierr);

    FEProblemInterface::init(get_dm());
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
    check_petsc_error(ierr);
    compute_postprocessors();
    output_step(sln);
    return 0;
}

PetscErrorCode
ImplicitFENonlinearProblem::ts_monitor_callback(PetscInt stepi, PetscReal t, Vec X)
{
    _F_;
    PetscErrorCode ierr;
    PetscReal dt;
    ierr = TSGetTimeStep(this->ts, &dt);
    check_petsc_error(ierr);
    this->time = t;
    this->step_num = stepi;
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
    // output initial condition
    output_step(this->x);
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
}

void
ImplicitFENonlinearProblem::output_step(Vec vec)
{
    _F_;
    PetscErrorCode ierr;
    DM dm = get_dm();

    for (auto & o : this->outputs)
        o->output_step(this->step_num, dm, vec);
}

} // namespace godzilla
