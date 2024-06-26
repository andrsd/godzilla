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
TSAbstract::compute_rhs_function(Real t, const Vector & U, Vector & y)
{
    CALL_STACK_MSG();
    this->tpi->compute_rhs_function(t, U, y);
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
