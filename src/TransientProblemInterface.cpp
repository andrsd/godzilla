#include "Godzilla.h"
#include "CallStack.h"
#include "Problem.h"
#include "TransientProblemInterface.h"
#include "NonlinearProblem.h"
#include "Output.h"
#include "petscdmplex.h"
#include <assert.h>

namespace godzilla {

PetscErrorCode
__transient_pre_step(TS ts)
{
    _F_;
    void * ctx;
    TSGetApplicationContext(ts, &ctx);
    TransientProblemInterface * tpi = static_cast<TransientProblemInterface *>(ctx);
    return tpi->pre_step();
}

PetscErrorCode
__transient_post_step(TS ts)
{
    _F_;
    void * ctx;
    TSGetApplicationContext(ts, &ctx);
    TransientProblemInterface * tpi = static_cast<TransientProblemInterface *>(ctx);
    return tpi->post_step();
}

PetscErrorCode
__transient_monitor(TS ts, PetscInt stepi, PetscReal time, Vec x, void * ctx)
{
    _F_;
    TransientProblemInterface * tpi = static_cast<TransientProblemInterface *>(ctx);
    return tpi->ts_monitor_callback(stepi, time, x);
}

InputParameters
TransientProblemInterface::valid_params()
{
    InputParameters params;
    params.add_param<PetscReal>("start_time", 0., "Start time of the simulation");
    params.add_required_param<PetscReal>("end_time", "Simulation end time");
    params.add_required_param<PetscReal>("dt", "Time step size");
    return params;
}

TransientProblemInterface::TransientProblemInterface(Problem * problem,
                                                     const InputParameters & params) :
    problem(problem),
    ts(nullptr),
    start_time(params.get<PetscReal>("start_time")),
    end_time(params.get<PetscReal>("end_time")),
    dt(params.get<PetscReal>("dt"))
{
    _F_;
    assert(this->problem != nullptr);
}

TransientProblemInterface::~TransientProblemInterface()
{
    _F_;
    TSDestroy(&this->ts);
}

void
TransientProblemInterface::init()
{
    _F_;
    PetscErrorCode ierr;
    ierr = TSCreate(this->problem->get_comm(), &this->ts);
    check_petsc_error(ierr);
    ierr = TSSetApplicationContext(this->ts, this);
    check_petsc_error(ierr);
}

void
TransientProblemInterface::create()
{
    _F_;
    PetscErrorCode ierr;

    set_up_time_scheme();

    ierr = TSSetDM(this->ts, this->problem->get_dm());
    check_petsc_error(ierr);

    ierr = TSSetTime(this->ts, this->start_time);
    check_petsc_error(ierr);
    ierr = TSSetMaxTime(this->ts, this->end_time);
    check_petsc_error(ierr);
    ierr = TSSetTimeStep(this->ts, this->dt);
    check_petsc_error(ierr);
    ierr = TSSetStepNumber(this->ts, this->problem->get_step_num());
    check_petsc_error(ierr);
    ierr = TSSetExactFinalTime(this->ts, TS_EXACTFINALTIME_MATCHSTEP);
    check_petsc_error(ierr);
}

void
TransientProblemInterface::set_up_monitors()
{
    _F_;
    PetscErrorCode ierr;
    ierr = TSSetPreStep(this->ts, __transient_pre_step);
    check_petsc_error(ierr);
    ierr = TSSetPostStep(this->ts, __transient_post_step);
    check_petsc_error(ierr);
    ierr = TSMonitorSet(this->ts, __transient_monitor, this, NULL);
    check_petsc_error(ierr);
}

void
TransientProblemInterface::set_up_time_scheme()
{
    _F_;
    PetscErrorCode ierr;
    // TODO: allow other schemes
    ierr = TSSetType(this->ts, TSBEULER);
    check_petsc_error(ierr);
}

PetscErrorCode
TransientProblemInterface::pre_step()
{
    _F_;
    return 0;
}

PetscErrorCode
TransientProblemInterface::post_step()
{
    _F_;
    PetscErrorCode ierr;

    ierr = TSGetTime(this->ts, &this->problem->time);
    check_petsc_error(ierr);
    ierr = TSGetStepNumber(this->ts, &this->problem->step_num);
    check_petsc_error(ierr);
    Vec sln;
    ierr = TSGetSolution(this->ts, &sln);
    check_petsc_error(ierr);
    ierr = VecCopy(sln, this->problem->get_solution_vector());
    check_petsc_error(ierr);
    this->problem->compute_postprocessors();
    this->problem->output(Output::ON_TIMESTEP);
    return 0;
}

PetscErrorCode
TransientProblemInterface::ts_monitor_callback(PetscInt stepi, PetscReal t, Vec x)
{
    _F_;
    PetscErrorCode ierr;
    PetscReal dt;
    ierr = TSGetTimeStep(this->ts, &dt);
    check_petsc_error(ierr);
    this->problem->lprintf(6, "%d Time %f dt = %f", stepi, t, dt);
    return 0;
}

void
TransientProblemInterface::solve(Vec x)
{
    _F_;
    TSSolve(this->ts, x);
}

} // namespace godzilla
