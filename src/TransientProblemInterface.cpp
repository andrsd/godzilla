// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Godzilla.h"
#include "godzilla/CallStack.h"
#include "godzilla/Problem.h"
#include "godzilla/TimeSteppingAdaptor.h"
#include "godzilla/TransientProblemInterface.h"
#include "godzilla/LoggingInterface.h"
#include "godzilla/Output.h"
#include <cassert>
#include "petsc/private/tsimpl.h"
#include "godzilla/SNESolver.h"

namespace godzilla {

ErrorCode
__transient_pre_step(TS ts)
{
    CALL_STACK_MSG();
    void * ctx;
    TSGetApplicationContext(ts, &ctx);
    auto * tpi = static_cast<TransientProblemInterface *>(ctx);
    tpi->pre_step();
    return 0;
}

ErrorCode
__transient_post_step(TS ts)
{
    CALL_STACK_MSG();
    void * ctx;
    TSGetApplicationContext(ts, &ctx);
    auto * tpi = static_cast<TransientProblemInterface *>(ctx);
    tpi->post_step();
    return 0;
}

ErrorCode
__transient_monitor(TS, Int stepi, Real time, Vec x, void * ctx)
{
    CALL_STACK_MSG();
    auto * tpi = static_cast<TransientProblemInterface *>(ctx);
    tpi->ts_monitor(stepi, time, x);
    return 0;
}

Parameters
TransientProblemInterface::parameters()
{
    Parameters params;
    params.add_param<Real>("start_time", 0., "Start time of the simulation");
    params.add_param<Real>("end_time", "Simulation end time");
    params.add_param<Int>("num_steps", "Number of steps");
    params.add_required_param<Real>("dt", "Time step size");
    params.add_param<std::map<std::string, std::string>>("ts_adapt", "Time stepping adaptivity");
    return params;
}

TransientProblemInterface::TransientProblemInterface(Problem * problem, const Parameters & params) :
    ts(nullptr),
    problem(problem),
    tpi_params(params),
    ts_adaptor(nullptr),
    start_time(params.get<Real>("start_time")),
    end_time(params.get<Real>("end_time")),
    num_steps(params.get<Int>("num_steps")),
    dt_initial(params.get<Real>("dt")),
    time(0.),
    step_num(0)
{
    CALL_STACK_MSG();
    PETSC_CHECK(TSCreate(this->problem->get_comm(), &this->ts));
    if (this->tpi_params.is_param_valid("end_time") && this->tpi_params.is_param_valid("num_steps"))
        this->problem->log_error(
            "Cannot provide 'end_time' and 'num_steps' together. Specify one or the other.");
    if (!this->tpi_params.is_param_valid("end_time") &&
        !this->tpi_params.is_param_valid("num_steps"))
        this->problem->log_error("You must provide either 'end_time' or 'num_steps' parameter.");
}

TransientProblemInterface::~TransientProblemInterface()
{
    CALL_STACK_MSG();
    TSDestroy(&this->ts);
}

SNESolver
TransientProblemInterface::get_snes() const
{
    CALL_STACK_MSG();
    SNES snes;
    PETSC_CHECK(TSGetSNES(this->ts, &snes));
    return SNESolver(snes);
}

void
TransientProblemInterface::set_time_stepping_adaptor(TimeSteppingAdaptor * adaptor)
{
    CALL_STACK_MSG();
    this->ts_adaptor = adaptor;
}

TimeSteppingAdaptor *
TransientProblemInterface::get_time_stepping_adaptor() const
{
    CALL_STACK_MSG();
    return this->ts_adaptor;
}

TS
TransientProblemInterface::get_ts() const
{
    CALL_STACK_MSG();
    return this->ts;
}

Vector
TransientProblemInterface::get_solution() const
{
    CALL_STACK_MSG();
    Vec sln;
    PETSC_CHECK(TSGetSolution(this->ts, &sln));
    return { sln };
}

Real
TransientProblemInterface::get_time_step() const
{
    CALL_STACK_MSG();
    Real dt;
    PETSC_CHECK(TSGetTimeStep(this->ts, &dt));
    return dt;
}

void
TransientProblemInterface::set_time_step(Real dt) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(TSSetTimeStep(this->ts, dt));
}

void
TransientProblemInterface::set_max_time(Real time)
{
    CALL_STACK_MSG();
    PETSC_CHECK(TSSetMaxTime(this->ts, time));
}

Real
TransientProblemInterface::get_max_time() const
{
    CALL_STACK_MSG();
    Real time;
    PETSC_CHECK(TSGetMaxTime(this->ts, &time));
    return time;
}

Real
TransientProblemInterface::get_time() const
{
    CALL_STACK_MSG();
    return this->time;
}

Int
TransientProblemInterface::get_step_number() const
{
    CALL_STACK_MSG();
    return this->step_num;
}

void
TransientProblemInterface::init()
{
    CALL_STACK_MSG();
    assert(this->problem != nullptr);
    PETSC_CHECK(TSSetDM(this->ts, this->problem->get_dm()));
    PETSC_CHECK(TSSetApplicationContext(this->ts, this));
}

void
TransientProblemInterface::create()
{
    CALL_STACK_MSG();
    set_up_time_scheme();
    PETSC_CHECK(TSSetTime(this->ts, this->start_time));
    if (this->tpi_params.is_param_valid("end_time"))
        set_max_time(this->end_time);
    if (this->tpi_params.is_param_valid("num_steps"))
        PETSC_CHECK(TSSetMaxSteps(this->ts, this->num_steps));
    set_time_step(this->dt_initial);
    PETSC_CHECK(TSSetStepNumber(this->ts, this->step_num));
    PETSC_CHECK(TSSetExactFinalTime(this->ts, TS_EXACTFINALTIME_MATCHSTEP));
    if (this->ts_adaptor)
        this->ts_adaptor->create();
}

void
TransientProblemInterface::set_up_monitors()
{
    CALL_STACK_MSG();
    PETSC_CHECK(TSSetPreStep(this->ts, __transient_pre_step));
    PETSC_CHECK(TSSetPostStep(this->ts, __transient_post_step));
    PETSC_CHECK(TSMonitorSet(this->ts, __transient_monitor, this, nullptr));
}

void
TransientProblemInterface::pre_step()
{
    CALL_STACK_MSG();
}

void
TransientProblemInterface::post_step()
{
    CALL_STACK_MSG();
    PETSC_CHECK(TSGetTime(this->ts, &this->time));
    PETSC_CHECK(TSGetStepNumber(this->ts, &this->step_num));
    Vector sln = get_solution();
    PETSC_CHECK(VecCopy(sln, this->problem->get_solution_vector()));
}

void
TransientProblemInterface::ts_monitor(Int stepi, Real time, Vec x)
{
    CALL_STACK_MSG();
    Real dt = get_time_step();
    this->problem->lprint(6, "{} Time {:f} dt = {:f}", stepi, time, dt);
}

void
TransientProblemInterface::solve(Vector & x)
{
    CALL_STACK_MSG();
    PETSC_CHECK(TSSolve(this->ts, x));
}

TSConvergedReason
TransientProblemInterface::get_converged_reason() const
{
    CALL_STACK_MSG();
    TSConvergedReason reason;
    PETSC_CHECK(TSGetConvergedReason(this->ts, &reason));
    return reason;
}

void
TransientProblemInterface::set_time(Real t)
{
    CALL_STACK_MSG();
    PETSC_CHECK(TSSetTime(this->ts, t));
    this->time = t;
}

void
TransientProblemInterface::pre_stage(Real stage_time)
{
}

void
TransientProblemInterface::post_stage(Real stage_time,
                                      Int stage_index,
                                      const std::vector<Vector> & Y)
{
}

ErrorCode
TransientProblemInterface::compute_rhs(Real time, const Vector & x, Vector & F)
{
    return 0;
}

void
TransientProblemInterface::set_converged_reason(TSConvergedReason reason)
{
    CALL_STACK_MSG();
    PETSC_CHECK(TSSetConvergedReason(this->ts, reason));
}

bool
TransientProblemInterface::converged() const
{
    CALL_STACK_MSG();
    return get_converged_reason() > 0;
}

void
TransientProblemInterface::set_scheme(TimeScheme scheme)
{
    CALL_STACK_MSG();
    if (scheme == TimeScheme::BEULER)
        PETSC_CHECK(TSSetType(this->ts, TSBEULER));
    else if (scheme == TimeScheme::CN)
        PETSC_CHECK(TSSetType(this->ts, TSCN));
    else if (scheme == TimeScheme::EULER)
        PETSC_CHECK(TSSetType(this->ts, TSEULER));
    else if (scheme == TimeScheme::SSP_RK_2) {
        PETSC_CHECK(TSSetType(this->ts, TSSSP));
        PETSC_CHECK(TSSSPSetType(this->ts, TSSSPRKS2));
    }
    else if (scheme == TimeScheme::SSP_RK_3) {
        PETSC_CHECK(TSSetType(this->ts, TSSSP));
        PETSC_CHECK(TSSSPSetType(this->ts, TSSSPRKS3));
    }
    else if (scheme == TimeScheme::RK_2) {
        PETSC_CHECK(TSSetType(this->ts, TSRK));
        PETSC_CHECK(TSRKSetType(this->ts, TSRK2B));
    }
    else if (scheme == TimeScheme::HEUN) {
        PETSC_CHECK(TSSetType(this->ts, TSRK));
        PETSC_CHECK(TSRKSetType(this->ts, TSRK2A));
    }
}

void
TransientProblemInterface::set_scheme(const std::string & scheme_name)
{
    CALL_STACK_MSG();
    PETSC_CHECK(TSSetType(this->ts, scheme_name.c_str()));
}

} // namespace godzilla
