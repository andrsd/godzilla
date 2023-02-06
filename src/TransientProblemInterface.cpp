#include "Godzilla.h"
#include "CallStack.h"
#include "Problem.h"
#include "TimeSteppingAdaptor.h"
#include "TransientProblemInterface.h"
#include "Output.h"
#include <cassert>
#include "petsc/private/tsimpl.h"

namespace godzilla {

PetscErrorCode
__transient_pre_step(TS ts)
{
    _F_;
    void * ctx;
    TSGetApplicationContext(ts, &ctx);
    auto * tpi = static_cast<TransientProblemInterface *>(ctx);
    return tpi->pre_step();
}

PetscErrorCode
__transient_post_step(TS ts)
{
    _F_;
    void * ctx;
    TSGetApplicationContext(ts, &ctx);
    auto * tpi = static_cast<TransientProblemInterface *>(ctx);
    return tpi->post_step();
}

PetscErrorCode
__transient_monitor(TS, Int stepi, Real time, Vec x, void * ctx)
{
    _F_;
    auto * tpi = static_cast<TransientProblemInterface *>(ctx);
    return tpi->ts_monitor_callback(stepi, time, x);
}

Parameters
TransientProblemInterface::parameters()
{
    Parameters params;
    params.add_param<Real>("start_time", 0., "Start time of the simulation");
    params.add_required_param<Real>("end_time", "Simulation end time");
    params.add_required_param<Real>("dt", "Time step size");
    params.add_param<std::map<std::string, std::string>>("ts_adapt", "Time stepping adaptivity");
    return params;
}

TransientProblemInterface::TransientProblemInterface(Problem * problem, const Parameters & params) :
    problem(problem),
    ts(nullptr),
    ts_adaptor(nullptr),
    start_time(params.get<Real>("start_time")),
    end_time(params.get<Real>("end_time")),
    dt(params.get<Real>("dt")),
    converged_reason(TS_CONVERGED_ITERATING)
{
    _F_;
}

TransientProblemInterface::~TransientProblemInterface()
{
    _F_;
    TSDestroy(&this->ts);
}

void
TransientProblemInterface::set_time_stepping_adaptor(TimeSteppingAdaptor * adaptor)
{
    _F_;
    this->ts_adaptor = adaptor;
}

TimeSteppingAdaptor *
TransientProblemInterface::get_time_stepping_adaptor() const
{
    _F_;
    return this->ts_adaptor;
}

TS
TransientProblemInterface::get_ts() const
{
    _F_;
    return this->ts;
}

void
TransientProblemInterface::init()
{
    _F_;
    assert(this->problem != nullptr);
    PETSC_CHECK(TSCreate(this->problem->get_comm(), &this->ts));
    PETSC_CHECK(TSSetDM(this->ts, this->problem->get_dm()));
    PETSC_CHECK(TSSetApplicationContext(this->ts, this));
}

void
TransientProblemInterface::create()
{
    _F_;
    set_up_time_scheme();
    PETSC_CHECK(TSSetTime(this->ts, this->start_time));
    PETSC_CHECK(TSSetMaxTime(this->ts, this->end_time));
    PETSC_CHECK(TSSetTimeStep(this->ts, this->dt));
    PETSC_CHECK(TSSetStepNumber(this->ts, this->problem->get_step_num()));
    PETSC_CHECK(TSSetExactFinalTime(this->ts, TS_EXACTFINALTIME_MATCHSTEP));
    if (this->ts_adaptor)
        this->ts_adaptor->create();
}

void
TransientProblemInterface::set_up_monitors()
{
    _F_;
    PETSC_CHECK(TSSetPreStep(this->ts, __transient_pre_step));
    PETSC_CHECK(TSSetPostStep(this->ts, __transient_post_step));
    PETSC_CHECK(TSMonitorSet(this->ts, __transient_monitor, this, nullptr));
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
    PETSC_CHECK(TSGetTime(this->ts, &this->problem->time));
    PETSC_CHECK(TSGetStepNumber(this->ts, &this->problem->step_num));
    Vec sln;
    PETSC_CHECK(TSGetSolution(this->ts, &sln));
    PETSC_CHECK(VecCopy(sln, this->problem->get_solution_vector()));
    this->problem->compute_postprocessors();
    this->problem->output(Output::ON_TIMESTEP);
    return 0;
}

PetscErrorCode
TransientProblemInterface::ts_monitor_callback(Int stepi, Real t, Vec x)
{
    _F_;
    Real dt;
    PETSC_CHECK(TSGetTimeStep(this->ts, &dt));
    this->problem->lprintf(6, "%d Time %f dt = %f", stepi, t, dt);
    return 0;
}

void
TransientProblemInterface::solve(Vec x)
{
    _F_;
    PETSC_CHECK(TSSolve(this->ts, x));
    PETSC_CHECK(TSGetConvergedReason(this->ts, &this->converged_reason));
}

bool
TransientProblemInterface::converged() const
{
    _F_;
    return this->converged_reason > 0;
}

} // namespace godzilla
