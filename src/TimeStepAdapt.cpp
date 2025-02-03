// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/TimeStepAdapt.h"
#include "godzilla/CallStack.h"
#include "godzilla/Error.h"
#include "godzilla/Exception.h"

namespace godzilla {

TimeStepAdapt::TimeStepAdapt() : tsadapt(nullptr) {}

TimeStepAdapt::TimeStepAdapt(TS ts, TSAdapt tsadapt) : ts(ts), tsadapt(tsadapt) {}

TS
TimeStepAdapt::get_ts() const
{
    CALL_STACK_MSG();
    return this->ts;
}

TSAdapt
TimeStepAdapt::get_ts_adapt() const
{
    CALL_STACK_MSG();
    return this->tsadapt;
}

void
TimeStepAdapt::add_candidate(const std::string & name,
                             Int order,
                             Int stage_order,
                             Real ccfl,
                             Real cost,
                             bool inuse)
{
    CALL_STACK_MSG();
    PETSC_CHECK(TSAdaptCandidateAdd(this->tsadapt,
                                    name.c_str(),
                                    order,
                                    stage_order,
                                    ccfl,
                                    cost,
                                    inuse ? PETSC_TRUE : PETSC_FALSE));
}

void
TimeStepAdapt::clear_candidates()
{
    CALL_STACK_MSG();
    PETSC_CHECK(TSAdaptCandidatesClear(this->tsadapt));
}

std::vector<TimeStepAdapt::Candidate>
TimeStepAdapt::get_candidates() const
{
    CALL_STACK_MSG();
    Int n;
    const Int * order;
    const Int * stage_order;
    const Real * ccfl;
    const Real * cost;
    PETSC_CHECK(TSAdaptCandidatesGet(this->tsadapt, &n, &order, &stage_order, &ccfl, &cost));
    std::vector<TimeStepAdapt::Candidate> candidates(n);
    for (Int i = 0; i < n; i++) {
        candidates[i].order = order[i];
        candidates[i].stage_order = stage_order[i];
        candidates[i].ccfl = ccfl[i];
        candidates[i].cost = cost[i];
    }
    return candidates;
}

bool
TimeStepAdapt::check_stage(Real t, const Vector & Y) const
{
    CALL_STACK_MSG();
    PetscBool accept;
    PETSC_CHECK(TSAdaptCheckStage(this->tsadapt, this->ts, t, Y, &accept));
    return accept == PETSC_TRUE;
}

std::tuple<Int, Real, bool>
TimeStepAdapt::choose(Real h)
{
    CALL_STACK_MSG();
    Int next_sc;
    Real next_h;
    PetscBool accept;
    PETSC_CHECK(TSAdaptChoose(this->tsadapt, this->ts, h, &next_sc, &next_h, &accept));
    return { next_sc, next_h, accept == PETSC_TRUE };
}

std::tuple<Real, Real>
TimeStepAdapt::get_clip() const
{
    CALL_STACK_MSG();
    Real low, high;
    PETSC_CHECK(TSAdaptGetClip(this->tsadapt, &low, &high));
    return { low, high };
}

Real
TimeStepAdapt::get_max_ignore() const
{
    CALL_STACK_MSG();
    Real max_ignore;
    PETSC_CHECK(TSAdaptGetMaxIgnore(this->tsadapt, &max_ignore));
    return max_ignore;
}

std::tuple<Real, Real>
TimeStepAdapt::get_safety() const
{
    CALL_STACK_MSG();
    Real safety, reject_safety;
    PETSC_CHECK(TSAdaptGetSafety(this->tsadapt, &safety, &reject_safety));
    return { safety, reject_safety };
}

Real
TimeStepAdapt::get_scale_solve_failed() const
{
    CALL_STACK_MSG();
    Real scale;
    PETSC_CHECK(TSAdaptGetScaleSolveFailed(this->tsadapt, &scale));
    return scale;
}

std::tuple<Real, Real>
TimeStepAdapt::get_step_limits() const
{
    CALL_STACK_MSG();
    Real hmin, hmax;
    PETSC_CHECK(TSAdaptGetStepLimits(this->tsadapt, &hmin, &hmax));
    return { hmin, hmax };
}

std::string
TimeStepAdapt::get_type() const
{
    CALL_STACK_MSG();
    TSAdaptType type;
    PETSC_CHECK(TSAdaptGetType(this->tsadapt, &type));
    return { type };
}

void
TimeStepAdapt::reset()
{
    CALL_STACK_MSG();
    PETSC_CHECK(TSAdaptReset(this->tsadapt));
}

void
TimeStepAdapt::set_always_accept(bool flag)
{
    CALL_STACK_MSG();
    PETSC_CHECK(TSAdaptSetAlwaysAccept(this->tsadapt, flag ? PETSC_TRUE : PETSC_FALSE));
}

void
TimeStepAdapt::set_clip(Real low, Real high)
{
    CALL_STACK_MSG();
    PETSC_CHECK(TSAdaptSetClip(this->tsadapt, low, high));
}

void
TimeStepAdapt::set_max_ignore(Real max_ignore)
{
    CALL_STACK_MSG();
    PETSC_CHECK(TSAdaptSetMaxIgnore(this->tsadapt, max_ignore));
}

void
TimeStepAdapt::set_safety(Real safety, Real reject_safety)
{
    CALL_STACK_MSG();
    PETSC_CHECK(TSAdaptSetSafety(this->tsadapt, safety, reject_safety));
}

void
TimeStepAdapt::set_scale_solve_failed(Real scale)
{
    CALL_STACK_MSG();
    PETSC_CHECK(TSAdaptSetScaleSolveFailed(this->tsadapt, scale));
}

void
TimeStepAdapt::set_step_limits(Real hmin, Real hmax)
{
    CALL_STACK_MSG();
    PETSC_CHECK(TSAdaptSetStepLimits(this->tsadapt, hmin, hmax));
}

void
TimeStepAdapt::set_type(const std::string & type)
{
    CALL_STACK_MSG();
    PETSC_CHECK(TSAdaptSetType(this->tsadapt, type.c_str()));
}

void
TimeStepAdapt::set_type(Type type)
{
    CALL_STACK_MSG();
    if (type == NONE)
        set_type(TSADAPTNONE);
    else if (type == BASIC)
        set_type(TSADAPTBASIC);
    else if (type == DSP)
        set_type(TSADAPTDSP);
    else if (type == CFL)
        set_type(TSADAPTCFL);
    else if (type == GLEE)
        set_type(TSADAPTGLEE);
    else if (type == HISTORY)
        set_type(TSADAPTHISTORY);
    else
        throw Exception("Unknown time stepping adaptivity method");
}

TimeStepAdapt
TimeStepAdapt::from_ts(TS ts)
{
    CALL_STACK_MSG();
    TSAdapt adapt;
    PETSC_CHECK(TSGetAdapt(ts, &adapt));
    return TimeStepAdapt(ts, adapt);
}

} // namespace godzilla
