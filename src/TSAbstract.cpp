// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/TSAbstract.h"
#include "godzilla/Error.h"
#include "godzilla/Vector.h"
#include "godzilla/CallStack.h"
#include "godzilla/Exception.h"
#include "godzilla/TransientProblemInterface.h"
#include "petscerror.h"

namespace godzilla {

TSAbstract::TSAbstract(TS ts) :
    ts(ts),
    adapt(TimeStepAdapt::from_ts(ts)),
    time_step(ts->time_step),
    ptime(ts->ptime),
    ptime_prev(ts->ptime_prev),
    steps(ts->steps),
    vec_sol(nullptr),
    reject(ts->reject),
    max_reject(ts->max_reject),
    status(TS_STEP_INCOMPLETE),
    reason(ts->reason)
{
    CALL_STACK_MSG();
    void * ctx;
    PETSC_CHECK(TSGetApplicationContext(this->ts, &ctx));
    this->tpi = static_cast<TransientProblemInterface *>(ctx);
    if (this->tpi == nullptr)
        throw InternalError("TS context is nullptr");
}

void
TSAbstract::set_status(TSStepStatus status)
{
    CALL_STACK_MSG();
    this->status = status;
}

TSStepStatus
TSAbstract::get_status() const
{
    CALL_STACK_MSG();
    return this->status;
}

Real
TSAbstract::get_time_step() const
{
    CALL_STACK_MSG();
    return this->time_step;
}

void
TSAbstract::set_time_step(Real h)
{
    CALL_STACK_MSG();
    this->time_step = h;
}

Real
TSAbstract::get_ptime() const
{
    CALL_STACK_MSG();
    return this->ptime;
}

void
TSAbstract::advance_ptime(Real h)
{
    CALL_STACK_MSG();
    this->ptime += h;
}

Real
TSAbstract::get_ptime_prev() const
{
    CALL_STACK_MSG();
    return this->ptime_prev;
}

Real
TSAbstract::get_max_reject() const
{
    CALL_STACK_MSG();
    return this->max_reject;
}

Int
TSAbstract::get_steps() const
{
    CALL_STACK_MSG();
    return this->steps;
}

const Vector &
TSAbstract::get_solution_vector() const
{
    CALL_STACK_MSG();
    return this->vec_sol;
}

Vector &
TSAbstract::get_solution_vector()
{
    CALL_STACK_MSG();
    return this->vec_sol;
}

const std::vector<Vector> &
TSAbstract::get_stage_vectors() const
{
    CALL_STACK_MSG();
    return this->Y;
}

std::vector<Vector> &
TSAbstract::get_stage_vectors()
{
    CALL_STACK_MSG();
    return this->Y;
}

void
TSAbstract::pre_stage(Real time)
{
    CALL_STACK_MSG();
    this->tpi->pre_stage(time);
}

void
TSAbstract::post_stage(Real stage_time, Int stage_index, const std::vector<Vector> & Y)
{
    CALL_STACK_MSG();
    this->tpi->post_stage(stage_time, stage_index, Y);
}

void
TSAbstract::set_up()
{
    CALL_STACK_MSG();
    this->vec_sol = Vector(this->ts->vec_sol);
}

void
TSAbstract::set_cfl_time_local(Real cfl)
{
    CALL_STACK_MSG();
    PETSC_CHECK(TSSetCFLTimeLocal(this->ts, cfl));
}

void
TSAbstract::compute_rhs(Real t, const Vector & U, Vector & y)
{
    CALL_STACK_MSG();
    this->tpi->compute_rhs(t, U, y);
}

TS
TSAbstract::get_ts()
{
    CALL_STACK_MSG();
    return this->ts;
}

TimeStepAdapt &
TSAbstract::get_adapt()
{
    CALL_STACK_MSG();
    return this->adapt;
}

const TimeStepAdapt &
TSAbstract::get_adapt() const
{
    CALL_STACK_MSG();
    return this->adapt;
}

TSConvergedReason
TSAbstract::get_reason() const
{
    CALL_STACK_MSG();
    return this->reason;
}

void
TSAbstract::set_reason(TSConvergedReason reason)
{
    CALL_STACK_MSG();
    this->reason = reason;
}

void
TSAbstract::inc_reject()
{
    CALL_STACK_MSG();
    this->reject++;
}

//

namespace internal {

ErrorCode
TSReset_GodzillaTS(TS ts)
{
    CALL_STACK_MSG();
    auto abstract_ts = static_cast<TSAbstract *>(ts->data);
    abstract_ts->reset();
    return 0;
}

ErrorCode
TSDestroy_GodzillaTS(TS ts)
{
    CALL_STACK_MSG();
    auto abstract_ts = static_cast<TSAbstract *>(ts->data);
    abstract_ts->reset();
    abstract_ts->destroy();
    delete abstract_ts;
    return 0;
}

ErrorCode
TSView_GodzillaTS(TS ts, PetscViewer viewer)
{
    CALL_STACK_MSG();
    auto abstract_ts = static_cast<TSAbstract *>(ts->data);
    abstract_ts->view(viewer);
    return 0;
}

ErrorCode
TSSetUp_GodzillaTS(TS ts)
{
    CALL_STACK_MSG();
    auto abstract_ts = static_cast<TSAbstract *>(ts->data);
    abstract_ts->set_up();
    return 0;
}

ErrorCode
TSRollBack_GodzillaTS(TS ts)
{
    CALL_STACK_MSG();
    auto abstract_ts = static_cast<TSAbstract *>(ts->data);
    abstract_ts->rollback();
    return 0;
}

ErrorCode
TSStep_GodzillaTS(TS ts)
{
    CALL_STACK_MSG();
    auto abstract_ts = static_cast<TSAbstract *>(ts->data);
    abstract_ts->step();
    return 0;
}

ErrorCode
TSEvaluateStep_GodzillaTS(TS ts, PetscInt order, Vec X, PetscBool * done)
{
    CALL_STACK_MSG();
    auto abstract_ts = static_cast<TSAbstract *>(ts->data);
    Vector vec_x(X);
    bool d;
    abstract_ts->evaluate_step(order, vec_x, done ? &d : nullptr);
    if (done)
        *done = d ? PETSC_TRUE : PETSC_FALSE;
    return 0;
}

} // namespace internal

} // namespace godzilla
