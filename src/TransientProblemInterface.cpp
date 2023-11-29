#include "godzilla/Godzilla.h"
#include "godzilla/CallStack.h"
#include "godzilla/Problem.h"
#include "godzilla/TimeSteppingAdaptor.h"
#include "godzilla/TransientProblemInterface.h"
#include "godzilla/LoggingInterface.h"
#include "godzilla/Output.h"
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
    tpi->pre_step();
    return 0;
}

PetscErrorCode
__transient_post_step(TS ts)
{
    _F_;
    void * ctx;
    TSGetApplicationContext(ts, &ctx);
    auto * tpi = static_cast<TransientProblemInterface *>(ctx);
    tpi->post_step();
    return 0;
}

PetscErrorCode
__transient_monitor(TS, Int stepi, Real time, Vec x, void * ctx)
{
    _F_;
    auto * tpi = static_cast<TransientProblemInterface *>(ctx);
    tpi->ts_monitor_callback(stepi, time, x);
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
    scheme(params.get<std::string>("scheme")),
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
    _F_;
}

TransientProblemInterface::~TransientProblemInterface()
{
    _F_;
    TSDestroy(&this->ts);
}

Problem *
TransientProblemInterface::get_problem()
{
    _F_;
    return this->problem;
}

SNES
TransientProblemInterface::get_snes() const
{
    SNES snes;
    PETSC_CHECK(TSGetSNES(this->ts, &snes));
    return snes;
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

const std::string &
TransientProblemInterface::get_scheme() const
{
    _F_;
    return this->scheme;
}

Vector
TransientProblemInterface::get_solution() const
{
    Vec sln;
    PETSC_CHECK(TSGetSolution(this->ts, &sln));
    return { sln };
}

Real
TransientProblemInterface::get_time_step() const
{
    _F_;
    Real dt;
    PETSC_CHECK(TSGetTimeStep(this->ts, &dt));
    return dt;
}

void
TransientProblemInterface::set_time_step(Real dt) const
{
    _F_;
    PETSC_CHECK(TSSetTimeStep(this->ts, dt));
}

void
TransientProblemInterface::set_max_time(Real time)
{
    _F_;
    PETSC_CHECK(TSSetMaxTime(this->ts, time));
}

Real
TransientProblemInterface::get_max_time() const
{
    _F_;
    Real time;
    PETSC_CHECK(TSGetMaxTime(this->ts, &time));
    return time;
}

Real
TransientProblemInterface::get_time() const
{
    _F_;
    return this->time;
}

Int
TransientProblemInterface::get_step_number() const
{
    _F_;
    return this->step_num;
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
TransientProblemInterface::check()
{
    _F_;
    if (this->tpi_params.is_param_valid("end_time") && this->tpi_params.is_param_valid("num_steps"))
        this->problem->log_error(
            "Cannot provide 'end_time' and 'num_steps' together. Specify one or the other.");
    if (!this->tpi_params.is_param_valid("end_time") &&
        !this->tpi_params.is_param_valid("num_steps"))
        this->problem->log_error("You must provide either 'end_time' or 'num_steps' parameter.");
}

void
TransientProblemInterface::set_up_monitors()
{
    _F_;
    PETSC_CHECK(TSSetPreStep(this->ts, __transient_pre_step));
    PETSC_CHECK(TSSetPostStep(this->ts, __transient_post_step));
    PETSC_CHECK(TSMonitorSet(this->ts, __transient_monitor, this, nullptr));
}

void
TransientProblemInterface::pre_step()
{
    _F_;
}

void
TransientProblemInterface::post_step()
{
    _F_;
    PETSC_CHECK(TSGetTime(this->ts, &this->time));
    PETSC_CHECK(TSGetStepNumber(this->ts, &this->step_num));
    Vector sln = get_solution();
    PETSC_CHECK(VecCopy(sln, this->problem->get_solution_vector()));
}

void
TransientProblemInterface::ts_monitor_callback(Int stepi, Real t, Vec x)
{
    _F_;
    Real dt = get_time_step();
    this->problem->lprint(6, "{} Time {:f} dt = {:f}", stepi, t, dt);
}

void
TransientProblemInterface::solve(Vector & x)
{
    _F_;
    PETSC_CHECK(TSSolve(this->ts, x));
}

TSConvergedReason
TransientProblemInterface::get_converged_reason() const
{
    _F_;
    TSConvergedReason reason;
    PETSC_CHECK(TSGetConvergedReason(this->ts, &reason));
    return reason;
}

void
TransientProblemInterface::set_converged_reason(TSConvergedReason reason)
{
    _F_;
    PETSC_CHECK(TSSetConvergedReason(this->ts, reason));
}

bool
TransientProblemInterface::converged() const
{
    _F_;
    return get_converged_reason() > 0;
}

void
TransientProblemInterface::set_scheme(TimeScheme scheme)
{
    _F_;
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
TransientProblemInterface::set_scheme(const char * scheme_name)
{
    _F_;
    PETSC_CHECK(TSSetType(this->ts, scheme_name));
}

} // namespace godzilla
