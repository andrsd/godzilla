// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "godzilla/Vector.h"
#include "godzilla/Delegate.h"
#include "godzilla/SNESolver.h"
#include "godzilla/Problem.h"
#include "godzilla/TSAbstract.h"
#include "petscts.h"
#include "petsc/private/tsimpl.h"

namespace godzilla {

class Problem;
class Parameters;
class TimeSteppingAdaptor;
class TimeStepAdapt;

/// Interface for transient simulations
///
class TransientProblemInterface {
public:
    enum ConvergedReason {
        CONVERGED_ITERATING = TS_CONVERGED_ITERATING,
        //
        CONVERGED_TIME = TS_CONVERGED_TIME,
        CONVERGED_ITS = TS_CONVERGED_ITS,
        CONVERGED_USER = TS_CONVERGED_USER,
        CONVERGED_EVENT = TS_CONVERGED_EVENT,
        CONVERGED_PSEUDO_FATOL = TS_CONVERGED_PSEUDO_FATOL,
        CONVERGED_PSEUDO_FRTOL = TS_CONVERGED_PSEUDO_FRTOL,
        //
        DIVERGED_NONLINEAR_SOLVE = TS_DIVERGED_NONLINEAR_SOLVE,
        DIVERGED_STEP_REJECTED = TS_DIVERGED_STEP_REJECTED,
        FORWARD_DIVERGED_LINEAR_SOLVE = TSFORWARD_DIVERGED_LINEAR_SOLVE,
        ADJOINT_DIVERGED_LINEAR_SOLVE = TSADJOINT_DIVERGED_LINEAR_SOLVE
    };

    /// Time stepping schemes
    enum class TimeScheme {
        /// Backward Euler
        BEULER,
        /// Crank-Nicolson
        CN,
        /// Forward Euler
        EULER,
        /// Runge-Kutta 2 (midpoint)
        RK_2,
        /// Heun's
        HEUN,
        /// Strong stability preserving RK-2
        SSP_RK_2,
        /// Strong stability preserving RK-3
        SSP_RK_3,
    };

    TransientProblemInterface(Problem * problem, const Parameters & params);
    virtual ~TransientProblemInterface();

    /// Set time stepping adaptivity using an adaptor class
    ///
    /// @param adaptor Time stepping adaptor object
    void set_time_stepping_adaptor(TimeSteppingAdaptor * adaptor);

    /// Get time stepping adaptor
    ///
    /// @return Time stepping adaptor
    TimeSteppingAdaptor * get_time_stepping_adaptor() const;

    /// Get TS object
    ///
    /// @return PETSc TS object
    TS get_ts() const;

    Vector get_solution() const;

    /// Get the current timestep size
    ///
    /// @return the current timestep size
    Real get_time_step() const;

    /// Allows one to reset the timestep at any time, useful for simple pseudo-timestepping codes.
    ///
    /// @param dt The size of the timestep
    void set_time_step(Real dt) const;

    /// Sets the maximum (or final) time for time-stepping
    ///
    /// @param max_time Final time to step to
    void set_max_time(Real max_time);

    /// Gets the maximum (or final) time for time-stepping
    ///
    /// @return Final time to step to
    Real get_max_time() const;

    /// Sets the reason for handling the convergence
    ///
    /// @param reason Converged reason
    void set_converged_reason(ConvergedReason reason);

    /// Gets the reason the time iteration was stopped
    ///
    /// NOTE: Can only be called after the call to solve() is complete.
    ///
    /// @return Converged reason
    ConvergedReason get_converged_reason() const;

    /// Set time-stepping scheme
    void set_scheme(TimeScheme scheme);

    /// Set time-stepping scheme
    void set_scheme(const std::string & scheme_name);

    /// Get the name of time stepping scheme
    std::string get_scheme() const;

    /// Set simulation time
    ///
    /// @param t New simulation time
    void set_time(Real t);

    /// Called before the time step solve
    virtual void pre_step();

    /// Called after the time step is done solving
    virtual void post_step();

    /// Runs the user-defined pre-stage function
    ///
    /// @param stage_time The absolute time of the current stage
    virtual void pre_stage(Real stage_time);

    /// Runs the user-defined post-stage function
    ///
    /// @param stage_time The absolute time of the current stage
    /// @param stage_index Stage number
    /// @param Y Array of vectors (of size = total number of stages) with the stage solutions
    virtual void post_stage(Real stage_time, Int stage_index, const std::vector<Vector> & Y);

    /// Compute right-hand side
    ///
    /// @param time Current time
    /// @param x Solution at time `time`
    /// @param F Right-hand side vector
    void compute_rhs(Real time, const Vector & x, Vector & F);

    /// Get a reference to the time stepper
    ///
    /// @tparam T Godzilla time stepper that inherits from `TSAbstract`
    /// @return Reference to the time stepper
    /// @note This won't work with internal PETSc time steppers
    template <class T>
    T &
    get_time_stepper()
    {
        CALL_STACK_MSG();
        return const_cast<T &>(*static_cast<T *>(this->ts->data));
    }

    /// Get a reference to the time stepper
    ///
    /// @return Reference to the (abstract) time stepper
    /// @note This won't work with internal PETSc time steppers, only with godzilla-registred ones.
    TSAbstract &
    get_time_stepper()
    {
        CALL_STACK_MSG();
        return *static_cast<TSAbstract *>(this->ts->data);
    }

    /// Get time step adaptivity
    ///
    /// @return Time step adaptivity object
    const TimeStepAdapt & get_time_step_adapt() const;

    /// Get time step adaptivity
    ///
    /// @return Time step adaptivity object
    TimeStepAdapt & get_time_step_adapt();

protected:
    /// Get underlying non-linear solver
    SNESolver get_snes() const;

    /// Get time
    Real get_time() const;

    /// Get step number
    Int get_step_number() const;

    /// Initialize
    void init();

    /// Create
    void create();

    /// Set up callbacks
    void set_up_callbacks();

    /// Set up monitors
    void set_up_monitors();

    /// Solve
    void solve(Vector & x);

    /// Sets an *additional* member function to be called at every iteration to monitor the
    /// residual/error etc.
    ///
    /// @tparam T C++ class type
    /// @param instance Instance of class T
    /// @param method Member function in class T
    template <class T>
    void
    monitor_set(T * instance, void (T::*method)(Int, Real, const Vector &))
    {
        this->monitor_method.bind(instance, method);
        PETSC_CHECK(
            TSMonitorSet(this->ts, invoke_monitor_delegate, &this->monitor_method, nullptr));
    }

    /// Sets the routine for evaluating the function, where U_t = G(t,u).
    ///
    /// @tparam T C++ class type
    /// @param instance Instance of class T
    /// @param method Member function in class T
    template <class T>
    void
    set_rhs_function(T * instance, void (T::*method)(Real time, const Vector & x, Vector & F))
    {
        this->compute_rhs_method.bind(instance, method);
        auto dm = this->problem->get_dm();
        PETSC_CHECK(DMTSSetRHSFunction(dm, invoke_compute_rhs_delegate, &this->compute_rhs_method));
    }

    template <class T>
    void
    set_ifunction_local(
        T * instance,
        void (T::*method)(Real time, const Vector & x, const Vector & x_t, Vector & F))
    {
        this->compute_ifunction_local_method.bind(instance, method);
        auto dm = this->problem->get_dm();
        PETSC_CHECK(DMTSSetIFunctionLocal(dm,
                                          invoke_compute_ifunction_delegate,
                                          &this->compute_ifunction_local_method));
    }

    template <class T>
    void
    set_ijacobian_local(
        T * instance,
        void (T::*method)(Real, const Vector &, const Vector &, Real, Matrix &, Matrix &))
    {
        this->compute_ijacobian_local_method.bind(instance, method);
        auto dm = this->problem->get_dm();
        PETSC_CHECK(DMTSSetIJacobianLocal(dm,
                                          invoke_compute_ijacobian_delegate,
                                          &this->compute_ijacobian_local_method));
    }

    /// Clears all the monitors that have been set on a time-stepping object.
    void monitor_cancel();

    /// Insert the essential boundary values into the local vector
    ///
    /// @param time The time
    /// @param x Local solution
    virtual void compute_boundary_local(Real time, Vector & x);

private:
    /// Monitor
    void monitor(Int stepi, Real time, const Vector & x);

    /// Set up time integration scheme
    virtual void set_up_time_scheme() = 0;

    /// PETSc TS object
    TS ts;
    /// Time step adapt object
    TimeStepAdapt time_step_adapt;
    /// Method for monitoring the solve
    Delegate<void(Int it, Real rnorm, const Vector & x)> monitor_method;
    /// Method for computing right-hand side
    Delegate<void(Real time, const Vector & x, Vector & F)> compute_rhs_method;
    /// Method for computing F(t,U,U_t) where F() = 0
    Delegate<void(Real time, const Vector & x, const Vector & x_t, Vector & F)>
        compute_ifunction_local_method;
    /// Method to compute the matrix dF/dU + a*dF/dU_t where F(t,U,U_t) is the function provided by
    /// `compute_ijacobian_local`
    Delegate<void(Real time,
                  const Vector & X,
                  const Vector & X_t,
                  Real x_t_shift,
                  Matrix & J,
                  Matrix & Jp)>
        compute_ijacobian_local_method;
    /// Problem this interface is part of
    Problem * problem;
    /// Parameters
    const Parameters & tpi_params;
    /// Time-stepping adaptor
    TimeSteppingAdaptor * ts_adaptor;
    /// Simulation start time
    const Real & start_time;
    /// Simulation end time
    const Real & end_time;
    /// Number of steps
    const Int & num_steps;
    /// Initial time step size
    const Real & dt_initial;
    /// Simulation time
    Real time;
    /// Time step number
    Int step_num;

public:
    static Parameters parameters();

private:
    static ErrorCode invoke_pre_step(TS ts);
    static ErrorCode invoke_post_step(TS ts);
    static ErrorCode invoke_monitor_delegate(TS ts, Int stepi, Real time, Vec x, void * ctx);
    static ErrorCode invoke_compute_rhs_delegate(TS, Real time, Vec x, Vec F, void * ctx);
    static ErrorCode
    invoke_compute_ifunction_delegate(DM, Real time, Vec x, Vec x_t, Vec F, void * context);
    static ErrorCode invoke_compute_ijacobian_delegate(DM,
                                                       Real time,
                                                       Vec x,
                                                       Vec x_t,
                                                       Real x_t_shift,
                                                       Mat J,
                                                       Mat Jp,
                                                       void * context);
};

} // namespace godzilla
