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
    ts_(ts),
    adapt_(TimeStepAdapt::from_ts(ts)),
    time_step_(ts->time_step),
    ptime_(ts->ptime),
    ptime_prev_(ts->ptime_prev),
    steps_(ts->steps),
    vec_sol_(nullptr),
    reject_(ts->reject),
    max_reject_(ts->max_reject),
    status_(TS_STEP_INCOMPLETE),
    reason_(ts->reason)
{
    CALL_STACK_MSG();
    void * ctx;
    PETSC_CHECK(TSGetApplicationContext(this->ts_, &ctx));
    this->tpi_ = static_cast<TransientProblemInterface *>(ctx);
    if (this->tpi_ == nullptr)
        throw InternalError("TS context is nullptr");
}

void
TSAbstract::set_status(TSStepStatus status)
{
    CALL_STACK_MSG();
    this->status_ = status;
}

TSStepStatus
TSAbstract::get_status() const
{
    CALL_STACK_MSG();
    return this->status_;
}

Real
TSAbstract::get_time_step() const
{
    CALL_STACK_MSG();
    return this->time_step_;
}

void
TSAbstract::set_time_step(Real h)
{
    CALL_STACK_MSG();
    this->time_step_ = h;
}

Real
TSAbstract::get_ptime() const
{
    CALL_STACK_MSG();
    return this->ptime_;
}

void
TSAbstract::advance_ptime(Real h)
{
    CALL_STACK_MSG();
    this->ptime_ += h;
}

Real
TSAbstract::get_ptime_prev() const
{
    CALL_STACK_MSG();
    return this->ptime_prev_;
}

Real
TSAbstract::get_max_reject() const
{
    CALL_STACK_MSG();
    return this->max_reject_;
}

Int
TSAbstract::get_steps() const
{
    CALL_STACK_MSG();
    return this->steps_;
}

const Vector &
TSAbstract::get_solution_vector() const
{
    CALL_STACK_MSG();
    return this->vec_sol_;
}

Vector &
TSAbstract::get_solution_vector()
{
    CALL_STACK_MSG();
    return this->vec_sol_;
}

const std::vector<Vector> &
TSAbstract::get_stage_vectors() const
{
    CALL_STACK_MSG();
    return this->Y_;
}

std::vector<Vector> &
TSAbstract::get_stage_vectors()
{
    CALL_STACK_MSG();
    return this->Y_;
}

void
TSAbstract::pre_stage(Real time)
{
    CALL_STACK_MSG();
    this->tpi_->pre_stage(time);
}

void
TSAbstract::post_stage(Real stage_time, Int stage_index, const std::vector<Vector> & Y)
{
    CALL_STACK_MSG();
    this->tpi_->post_stage(stage_time, stage_index, Y);
}

void
TSAbstract::set_up()
{
    CALL_STACK_MSG();
    this->vec_sol_ = Vector(this->ts_->vec_sol);
    this->vec_sol_.inc_reference();
}

void
TSAbstract::set_cfl_time_local(Real cfl)
{
    CALL_STACK_MSG();
    PETSC_CHECK(TSSetCFLTimeLocal(this->ts_, cfl));
}

void
TSAbstract::compute_rhs(Real t, const Vector & U, Vector & y)
{
    CALL_STACK_MSG();
    this->tpi_->compute_rhs(t, U, y);
}

TS
TSAbstract::get_ts()
{
    CALL_STACK_MSG();
    return this->ts_;
}

TimeStepAdapt &
TSAbstract::get_adapt()
{
    CALL_STACK_MSG();
    return this->adapt_;
}

const TimeStepAdapt &
TSAbstract::get_adapt() const
{
    CALL_STACK_MSG();
    return this->adapt_;
}

TSConvergedReason
TSAbstract::get_reason() const
{
    CALL_STACK_MSG();
    return this->reason_;
}

void
TSAbstract::set_reason(TSConvergedReason reason)
{
    CALL_STACK_MSG();
    this->reason_ = reason;
}

void
TSAbstract::inc_reject()
{
    CALL_STACK_MSG();
    ++this->reject_;
}

//

namespace internal {

PetscErrorCode
TSReset_GodzillaTS(TS ts)
{
    CALL_STACK_MSG();
    auto abstract_ts = static_cast<TSAbstract *>(ts->data);
    abstract_ts->reset();
    return 0;
}

PetscErrorCode
TSDestroy_GodzillaTS(TS ts)
{
    CALL_STACK_MSG();
    auto abstract_ts = static_cast<TSAbstract *>(ts->data);
    abstract_ts->reset();
    abstract_ts->destroy();
    delete abstract_ts;
    return 0;
}

PetscErrorCode
TSView_GodzillaTS(TS ts, PetscViewer viewer)
{
    CALL_STACK_MSG();
    auto abstract_ts = static_cast<TSAbstract *>(ts->data);
    abstract_ts->view(viewer);
    return 0;
}

PetscErrorCode
TSSetUp_GodzillaTS(TS ts)
{
    CALL_STACK_MSG();
    auto abstract_ts = static_cast<TSAbstract *>(ts->data);
    abstract_ts->set_up();
    return 0;
}

PetscErrorCode
TSRollBack_GodzillaTS(TS ts)
{
    CALL_STACK_MSG();
    auto abstract_ts = static_cast<TSAbstract *>(ts->data);
    abstract_ts->rollback();
    return 0;
}

PetscErrorCode
TSStep_GodzillaTS(TS ts)
{
    CALL_STACK_MSG();
    auto abstract_ts = static_cast<TSAbstract *>(ts->data);
    abstract_ts->step();
    return 0;
}

PetscErrorCode
TSEvaluateStep_GodzillaTS(TS ts, PetscInt order, Vec X, PetscBool * done)
{
    CALL_STACK_MSG();
    auto abstract_ts = static_cast<TSAbstract *>(ts->data);
    Vector vec_x(X);
    vec_x.inc_reference();
    bool d = false;
    abstract_ts->evaluate_step(order, vec_x, done ? &d : nullptr);
    if (done)
        *done = d ? PETSC_TRUE : PETSC_FALSE;
    return 0;
}

} // namespace internal

} // namespace godzilla
