// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/CallStack.h"
#include "godzilla/Problem.h"
#include "godzilla/TimeStepAdapt.h"
#include "godzilla/TransientProblemInterface.h"
#include "godzilla/Assert.h"
#include "godzilla/SNESolver.h"
#include "petscdmplex.h"

namespace godzilla {

ErrorCode
TransientProblemInterface::invoke_pre_step(TS ts)
{
    CALL_STACK_MSG();
    void * ctx;
    PETSC_CHECK(TSGetApplicationContext(ts, &ctx));
    auto * tpi = static_cast<TransientProblemInterface *>(ctx);
    tpi->pre_step();
    return 0;
}

ErrorCode
TransientProblemInterface::invoke_post_step(TS ts)
{
    CALL_STACK_MSG();
    void * ctx;
    PETSC_CHECK(TSGetApplicationContext(ts, &ctx));
    auto * tpi = static_cast<TransientProblemInterface *>(ctx);
    tpi->post_step();
    return 0;
}

ErrorCode
TransientProblemInterface::invoke_monitor_delegate(TS, Int stepi, Real time, Vec x, void * ctx)
{
    CALL_STACK_MSG();
    auto * method = static_cast<Delegate<void(Int it, Real rnorm, const Vector & x)> *>(ctx);
    Vector vec_x(x);
    vec_x.inc_reference();
    method->invoke(stepi, time, vec_x);
    return 0;
}

ErrorCode
TransientProblemInterface::invoke_compute_rhs_function_delegate(TS,
                                                                Real time,
                                                                Vec x,
                                                                Vec F,
                                                                void * ctx)
{
    CALL_STACK_MSG();
    auto * method = static_cast<Delegate<void(Real time, const Vector & x, Vector & F)> *>(ctx);
    Vector vec_x(x);
    vec_x.inc_reference();
    Vector vec_F(F);
    vec_F.inc_reference();
    method->invoke(time, vec_x, vec_F);
    return 0;
}

ErrorCode
TransientProblemInterface::invoke_compute_rhs_jacobian_delegate(TS,
                                                                Real time,
                                                                Vec x,
                                                                Mat A,
                                                                Mat B,
                                                                void * ctx)
{
    CALL_STACK_MSG();
    auto * method =
        static_cast<Delegate<void(Real time, const Vector & x, Matrix & A, Matrix & B)> *>(ctx);
    Vector vec_x(x);
    vec_x.inc_reference();
    Matrix mat_A(A);
    mat_A.inc_reference();
    Matrix mat_B(B);
    mat_B.inc_reference();
    method->invoke(time, vec_x, mat_A, mat_B);
    return 0;
}

ErrorCode
TransientProblemInterface::invoke_compute_ifunction_delegate(TS,
                                                             Real time,
                                                             Vec x,
                                                             Vec x_t,
                                                             Vec F,
                                                             void * context)
{
    auto * method =
        static_cast<Delegate<void(Real, const Vector &, const Vector &, Vector &)> *>(context);
    Vector vec_x(x);
    vec_x.inc_reference();
    Vector vec_x_t(x_t);
    vec_x_t.inc_reference();
    Vector vec_F(F);
    vec_F.inc_reference();
    method->invoke(time, vec_x, vec_x_t, vec_F);
    return 0;
}

ErrorCode
TransientProblemInterface::invoke_compute_ijacobian_delegate(TS,
                                                             Real time,
                                                             Vec x,
                                                             Vec x_t,
                                                             Real x_t_shift,
                                                             Mat J,
                                                             Mat Jp,
                                                             void * context)
{
    CALL_STACK_MSG();
    auto * method = static_cast<
        Delegate<void(Real time, const Vector &, const Vector &, Real, Matrix &, Matrix &)> *>(
        context);
    Vector vec_x(x);
    vec_x.inc_reference();
    Vector vec_x_t(x_t);
    vec_x_t.inc_reference();
    Matrix mat_J(J);
    mat_J.inc_reference();
    Matrix mat_Jp(Jp);
    mat_Jp.inc_reference();
    method->invoke(time, vec_x, vec_x_t, x_t_shift, mat_J, mat_Jp);
    return 0;
}

ErrorCode
TransientProblemInterface::invoke_compute_ifunction_delegate(DM,
                                                             Real time,
                                                             Vec x,
                                                             Vec x_t,
                                                             Vec F,
                                                             void * contex)
{
    CALL_STACK_MSG();
    auto * method =
        static_cast<Delegate<void(Real time, const Vector & x, const Vector & x_t, Vector & F)> *>(
            contex);
    Vector vec_x(x);
    vec_x.inc_reference();
    Vector vec_x_t(x_t);
    vec_x_t.inc_reference();
    Vector vec_F(F);
    vec_F.inc_reference();
    method->invoke(time, vec_x, vec_x_t, vec_F);
    return 0;
}

ErrorCode
TransientProblemInterface::invoke_compute_ijacobian_delegate(DM,
                                                             Real time,
                                                             Vec x,
                                                             Vec x_t,
                                                             Real x_t_shift,
                                                             Mat J,
                                                             Mat Jp,
                                                             void * contex)
{
    CALL_STACK_MSG();
    auto * method = static_cast<Delegate<void(Real time,
                                              const Vector & X,
                                              const Vector & X_t,
                                              Real x_t_shift,
                                              Matrix & J,
                                              Matrix & Jp)> *>(contex);
    Vector vec_x(x);
    vec_x.inc_reference();
    Vector vec_x_t(x_t);
    vec_x_t.inc_reference();
    Matrix mat_J(J);
    mat_J.inc_reference();
    Matrix mat_Jp(Jp);
    mat_Jp.inc_reference();
    method->invoke(time, vec_x, vec_x_t, x_t_shift, mat_J, mat_Jp);
    return 0;
}

//

Parameters
TransientProblemInterface::parameters()
{
    Parameters params;
    params.add_param<Real>("start_time", 0., "Start time of the simulation");
    params.add_param<Real>("end_time", "Simulation end time");
    params.add_param<Int>("num_steps", "Number of steps");
    params.add_required_param<Real>("dt", "Time step size");
    return params;
}

TransientProblemInterface::TransientProblemInterface(Problem * problem, const Parameters & pars) :
    ts(nullptr),
    problem(problem),
    start_time(pars.get<Real>("start_time")),
    end_time(pars.get<Optional<Real>>("end_time")),
    num_steps(pars.get<Optional<Int>>("num_steps")),
    dt_initial(pars.get<Real>("dt")),
    step_num(0)
{
    CALL_STACK_MSG();
    PETSC_CHECK(TSCreate(this->problem->get_comm(), &this->ts));
    PETSC_CHECK(TSSetApplicationContext(this->ts, this));
    this->time_step_adapt = TimeStepAdapt::from_ts(this->ts);

    if (this->end_time.has_value() && this->num_steps.has_value())
        this->problem->log_error(
            "Cannot provide 'end_time' and 'num_steps' together. Specify one or the other.");
    if (!this->end_time.has_value() && !this->num_steps.has_value())
        this->problem->log_error("You must provide either 'end_time' or 'num_steps' parameter.");
}

TransientProblemInterface::~TransientProblemInterface()
{
    CALL_STACK_MSG();
    PETSC_CHECK(TSDestroy(&this->ts));
}

SNESolver
TransientProblemInterface::get_snes() const
{
    CALL_STACK_MSG();
    SNESolver snes;
    PETSC_CHECK(TSGetSNES(this->ts, snes));
    snes.inc_reference();
    return snes;
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
    Vector sln;
    PETSC_CHECK(TSGetSolution(this->ts, sln));
    sln.inc_reference();
    return sln;
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
    Real time;
    PETSC_CHECK(TSGetTime(this->ts, &time));
    return time;
}

Int
TransientProblemInterface::get_step_number() const
{
    CALL_STACK_MSG();
    return this->step_num;
}

const TimeStepAdapt &
TransientProblemInterface::get_time_step_adapt() const
{
    CALL_STACK_MSG();
    return this->time_step_adapt;
}

TimeStepAdapt &
TransientProblemInterface::get_time_step_adapt()
{
    CALL_STACK_MSG();
    return this->time_step_adapt;
}

void
TransientProblemInterface::init()
{
    CALL_STACK_MSG();
    assert_true(this->problem != nullptr, "Problem is null");
    PETSC_CHECK(TSSetDM(this->ts, this->problem->get_dm()));
}

void
TransientProblemInterface::create()
{
    CALL_STACK_MSG();
    set_up_time_scheme();
    set_time(this->start_time);
    if (this->end_time.has_value())
        set_max_time(this->end_time.value());
    if (this->num_steps.has_value())
        PETSC_CHECK(TSSetMaxSteps(this->ts, this->num_steps.value()));
    set_time_step(this->dt_initial);
    PETSC_CHECK(TSSetStepNumber(this->ts, this->step_num));
    PETSC_CHECK(TSSetExactFinalTime(this->ts, TS_EXACTFINALTIME_MATCHSTEP));
}

void
TransientProblemInterface::set_up_callbacks()
{
    CALL_STACK_MSG();
    PETSC_CHECK(TSSetPreStep(this->ts, invoke_pre_step));
    PETSC_CHECK(TSSetPostStep(this->ts, invoke_post_step));
}

void
TransientProblemInterface::set_up_monitors()
{
    CALL_STACK_MSG();
    monitor_set(this, &TransientProblemInterface::monitor);
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
    PETSC_CHECK(TSGetStepNumber(this->ts, &this->step_num));
    Vector sln = get_solution();
    PETSC_CHECK(VecCopy(sln, this->problem->get_solution_vector()));
}

void
TransientProblemInterface::monitor(Int stepi, Real time, const Vector & x)
{
    CALL_STACK_MSG();
    Real dt = get_time_step();
    this->problem->lprintln(6, "{} Time {:f} dt = {:f}", stepi, time, dt);
}

void
TransientProblemInterface::solve(Vector & x)
{
    CALL_STACK_MSG();
    PETSC_CHECK(TSSolve(this->ts, x));
}

TransientProblemInterface::ConvergedReason
TransientProblemInterface::get_converged_reason() const
{
    CALL_STACK_MSG();
    TSConvergedReason reason;
    PETSC_CHECK(TSGetConvergedReason(this->ts, &reason));
    return static_cast<ConvergedReason>(reason);
}

void
TransientProblemInterface::set_time(Real t)
{
    CALL_STACK_MSG();
    PETSC_CHECK(TSSetTime(this->ts, t));
}

auto
TransientProblemInterface::get_problem_type() const -> ProblemType
{
    CALL_STACK_MSG();
    TSProblemType tpt;
    PETSC_CHECK(TSGetProblemType(this->ts, &tpt));
    return static_cast<ProblemType>(tpt);
}

void
TransientProblemInterface::set_problem_type(ProblemType type)
{
    PETSC_CHECK(TSSetProblemType(this->ts, static_cast<TSProblemType>(type)));
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

void
TransientProblemInterface::compute_rhs(Real time, const Vector & x, Vector & F)
{
    CALL_STACK_MSG();
    if (this->compute_rhs_function_method)
        this->compute_rhs_function_method(time, x, F);
}

void
TransientProblemInterface::set_converged_reason(ConvergedReason reason)
{
    CALL_STACK_MSG();
    PETSC_CHECK(TSSetConvergedReason(this->ts, static_cast<TSConvergedReason>(reason)));
}

void
TransientProblemInterface::set_scheme(String scheme_name)
{
    CALL_STACK_MSG();
    PETSC_CHECK(TSSetType(this->ts, scheme_name.c_str()));
}

void
TransientProblemInterface::set_scheme(String scheme_name, String sub_name)
{
    PETSC_CHECK(TSSetType(this->ts, scheme_name.c_str()));
    if (scheme_name == TSSSP)
        PETSC_CHECK(TSSSPSetType(this->ts, sub_name.c_str()));
    else if (scheme_name == TSRK)
        PETSC_CHECK(TSRKSetType(this->ts, sub_name.c_str()));
}

String
TransientProblemInterface::get_scheme() const
{
    CALL_STACK_MSG();
    TSType type;
    TSGetType(this->ts, &type);
    return { type };
}

void
TransientProblemInterface::monitor_cancel()
{
    CALL_STACK_MSG();
    PETSC_CHECK(TSMonitorCancel(this->ts));
    this->monitor_method.reset();
}

bool
TransientProblemInterface::function_domain_error(Real stage_time, const Vector & Y)
{
    CALL_STACK_MSG();
    PetscBool ok;
    PETSC_CHECK(TSFunctionDomainError(this->ts, stage_time, Y, &ok));
    return ok == PETSC_TRUE ? true : false;
}

} // namespace godzilla
