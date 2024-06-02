// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Godzilla.h"
#include "godzilla/CallStack.h"
#include "godzilla/Problem.h"
#include "godzilla/TimeSteppingAdaptor.h"
#include "godzilla/TransientProblemInterface.h"
#include "godzilla/LoggingInterface.h"
#include "godzilla/Output.h"
#include "godzilla/SNESolver.h"
#include "petsc/private/tsimpl.h"
#include <cassert>

namespace godzilla {

ErrorCode
TransientProblemInterface::pre_step(TS ts)
{
    CALL_STACK_MSG();
    void * ctx;
    TSGetApplicationContext(ts, &ctx);
    auto * tpi = static_cast<TransientProblemInterface *>(ctx);
    tpi->pre_step();
    return 0;
}

ErrorCode
TransientProblemInterface::post_step(TS ts)
{
    CALL_STACK_MSG();
    void * ctx;
    TSGetApplicationContext(ts, &ctx);
    auto * tpi = static_cast<TransientProblemInterface *>(ctx);
    tpi->post_step();
    return 0;
}

ErrorCode
TransientProblemInterface::monitor(TS, Int stepi, Real time, Vec x, void * ctx)
{
    CALL_STACK_MSG();
    auto * method = static_cast<internal::TSMonitorMethodAbstract *>(ctx);
    Vector vec_x(x);
    return method->invoke(stepi, time, vec_x);
}

ErrorCode
TransientProblemInterface::monitor_destroy(void ** ctx)
{
    auto * method = static_cast<internal::TSMonitorMethodAbstract *>(*ctx);
    delete method;
    return 0;
}

ErrorCode
TransientProblemInterface::compute_rhs(TS, Real time, Vec x, Vec F, void * ctx)
{
    CALL_STACK_MSG();
    auto * method = static_cast<internal::TSComputeRhsMethodAbstract *>(ctx);
    Vector vec_x(x);
    Vector vec_F(F);
    return method->invoke(time, vec_x, vec_F);
}

ErrorCode
TransientProblemInterface::compute_ifunction(DM, Real time, Vec x, Vec x_t, Vec F, void * contex)
{
    CALL_STACK_MSG();
    auto * method = static_cast<internal::TSComputeIFunctionMethodAbstract *>(contex);
    Vector vec_x(x);
    Vector vec_x_t(x_t);
    Vector vec_F(F);
    return method->invoke(time, vec_x, vec_x_t, vec_F);
}

ErrorCode
TransientProblemInterface::compute_ijacobian(DM,
                                             Real time,
                                             Vec x,
                                             Vec x_t,
                                             Real x_t_shift,
                                             Mat J,
                                             Mat Jp,
                                             void * contex)
{
    CALL_STACK_MSG();
    auto * method = static_cast<internal::TSComputeIJacobianMethodAbstract *>(contex);
    Vector vec_x(x);
    Vector vec_x_t(x_t);
    Matrix mat_J(J);
    Matrix mat_Jp(Jp);
    method->invoke(time, vec_x, vec_x_t, x_t_shift, mat_J, mat_Jp);
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
    compute_rhs_method(nullptr),
    compute_ifunction_local_method(nullptr),
    compute_ijacobian_local_method(nullptr),
    monitor_method(nullptr),
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
TransientProblemInterface::set_up_callbacks()
{
    CALL_STACK_MSG();
    PETSC_CHECK(TSSetPreStep(this->ts, TransientProblemInterface::pre_step));
    PETSC_CHECK(TSSetPostStep(this->ts, TransientProblemInterface::post_step));
}

void
TransientProblemInterface::set_up_monitors()
{
    CALL_STACK_MSG();
    monitor_set(this, &TransientProblemInterface::default_monitor);
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

ErrorCode
TransientProblemInterface::default_monitor(Int stepi, Real time, const Vector & x)
{
    CALL_STACK_MSG();
    Real dt = get_time_step();
    this->problem->lprint(6, "{} Time {:f} dt = {:f}", stepi, time, dt);
    return 0;
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
TransientProblemInterface::compute_rhs_function(Real time, const Vector & x, Vector & F)
{
    CALL_STACK_MSG();
    if (this->compute_rhs_method)
        return this->compute_rhs_method->invoke(time, x, F);
    else
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

void
TransientProblemInterface::monitor_cancel()
{
    CALL_STACK_MSG();
    PETSC_CHECK(TSMonitorCancel(this->ts));
}

} // namespace godzilla
