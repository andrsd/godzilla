// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "godzilla/Vector.h"
#include "godzilla/TimeStepAdapt.h"
#include "petscts.h"
#include "petsc/private/tsimpl.h"
#include <vector>

namespace godzilla {

class TSAbstract {
public:
    explicit TSAbstract(TS ts);
    virtual ~TSAbstract() = default;
    virtual void destroy() = 0;
    virtual void reset() = 0;
    virtual void set_up();
    virtual void step() = 0;
    virtual void evaluate_step(Int order, Vector & X, bool * done) = 0;
    virtual void rollback() = 0;
    virtual void view(PetscViewer viewer) = 0;

    const std::vector<Vector> & get_stage_vectors() const;
    std::vector<Vector> & get_stage_vectors();

    /// Runs the user-defined pre-stage function set using
    ///
    /// @param time The absolute time of the current stage
    void pre_stage(Real time);

    /// Runs the user-defined post-stage function
    ///
    /// @param stage_time The absolute time of the current stage
    /// @param stage_index Stage number
    /// @param Y Array of vectors (of size = total number of stages) with the stage solutions
    void post_stage(Real stage_time, Int stage_index, const std::vector<Vector> & Y);

    /// Set the local CFL constraint relative to forward Euler
    ///
    /// @param cfl Maximum stable time step if using forward Euler (value can be different on each
    /// process)
    void set_cfl_time_local(Real cfl);

    /// Evaluates the right-hand-side function
    ///
    /// @param t Current time
    /// @param U State vector
    /// @param y Right-hand side
    void compute_rhs_function(Real t, const Vector & U, Vector & y);

protected:
    /// PETSc TS object
    TS ts;
    /// Time stepping adaptivity
    TimeStepAdapt adapt;
    /// Current time increment
    Real & time_step;
    /// Time at the start of the current step (stage time is internal if it exists)
    Real & ptime;
    /// Time at the start of the previous step
    Real & ptime_prev;
    /// Steps taken so far in all successive calls to TSSolve()
    Int & steps;
    /// Solution vector in first and second order equations
    Vector vec_sol;
    ///
    Int & reject;
    ///
    Int & max_reject;
    /// Status of the step
    TSStepStatus status;
    /// Converged reason
    TSConvergedReason & reason;

    /// States computed during the step
    std::vector<Vector> Y;
};

namespace internal {

PetscErrorCode TSReset_GodzillaTS(TS ts);
PetscErrorCode TSDestroy_GodzillaTS(TS ts);
PetscErrorCode TSView_GodzillaTS(TS ts, PetscViewer viewer);
PetscErrorCode TSSetUp_GodzillaTS(TS ts);
PetscErrorCode TSRollBack_GodzillaTS(TS ts);
PetscErrorCode TSStep_GodzillaTS(TS ts);
PetscErrorCode TSEvaluateStep_GodzillaTS(TS ts, PetscInt order, Vec X, PetscBool * done);

} // namespace internal

template <class T>
PetscErrorCode
create(TS ts)
{
    ts->ops->reset = internal::TSReset_GodzillaTS;
    ts->ops->destroy = internal::TSDestroy_GodzillaTS;
    ts->ops->view = internal::TSView_GodzillaTS;
    ts->ops->load = nullptr;
    ts->ops->setup = internal::TSSetUp_GodzillaTS;
    ts->ops->interpolate = nullptr;
    ts->ops->step = internal::TSStep_GodzillaTS;
    ts->ops->evaluatestep = internal::TSEvaluateStep_GodzillaTS;
    ts->ops->rollback = internal::TSRollBack_GodzillaTS;
    ts->ops->setfromoptions = nullptr;
    ts->ops->getstages = nullptr;

    ts->ops->snesfunction = nullptr;
    ts->ops->snesjacobian = nullptr;
    ts->ops->adjointintegral = nullptr;
    ts->ops->adjointsetup = nullptr;
    ts->ops->adjointstep = nullptr;
    ts->ops->adjointreset = nullptr;

    ts->ops->forwardintegral = nullptr;
    ts->ops->forwardsetup = nullptr;
    ts->ops->forwardreset = nullptr;
    ts->ops->forwardstep = nullptr;
    ts->ops->forwardgetstages = nullptr;

    ts->data = (void *) new T(ts);
    return 0;
}

template <class T>
void
register_ts()
{
    TSRegister(T::name.c_str(), &create<T>);
}

} // namespace godzilla
