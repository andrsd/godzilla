// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "petscts.h"
#include "godzilla/Types.h"
#include "godzilla/Vector.h"
#include "godzilla/Delegate.h"
#include "godzilla/SNESolver.h"
#include "godzilla/Problem.h"

namespace godzilla {

class Problem;
class Parameters;
class TimeSteppingAdaptor;

/// Interface for transient simulations
///
class TransientProblemInterface {
public:
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
    [[nodiscard]] TimeSteppingAdaptor * get_time_stepping_adaptor() const;

    /// Get TS object
    ///
    /// @return PETSc TS object
    [[nodiscard]] TS get_ts() const;

    [[nodiscard]] Vector get_solution() const;

    /// Get the current timestep size
    ///
    /// @return the current timestep size
    [[nodiscard]] Real get_time_step() const;

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
    [[nodiscard]] Real get_max_time() const;

    /// Sets the reason for handling the convergence
    ///
    /// @param reason Converged reason
    void set_converged_reason(TSConvergedReason reason);

    /// Gets the reason the time iteration was stopped
    ///
    /// NOTE: Can only be called after the call to solve() is complete.
    ///
    /// @return Converged reason
    [[nodiscard]] TSConvergedReason get_converged_reason() const;

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
    void compute_rhs_function(Real time, const Vector & x, Vector & F);

protected:
    /// Get underlying non-linear solver
    [[nodiscard]] SNESolver get_snes() const;
    /// Get time
    [[nodiscard]] Real get_time() const;
    /// Get step number
    [[nodiscard]] Int get_step_number() const;
    /// Initialize
    void init();
    /// Create
    void create();
    /// Set up callbacks
    void set_up_callbacks();
    /// Set up monitors
    void set_up_monitors();
    /// Set up time integration scheme
    virtual void set_up_time_scheme() = 0;
    /// Default TS monitor
    ErrorCode default_monitor(Int stepi, Real time, const Vector & x);
    /// Check if problem converged
    ///
    /// @return `true` if solve converged, otherwise `false`
    [[nodiscard]] bool converged() const;
    /// Solve
    void solve(Vector & x);
    /// Set time-stepping scheme
    void set_scheme(TimeScheme scheme);
    /// Set time-stepping scheme
    void set_scheme(const std::string & scheme_name);

    /// Sets an *additional* member function to be called at every iteration to monitor the
    /// residual/error etc.
    ///
    /// @tparam T C++ class type
    /// @param instance Instance of class T
    /// @param method Member function in class T
    template <class T>
    void
    monitor_set(T * instance, ErrorCode (T::*method)(Int, Real, const Vector &))
    {
        this->monitor_method.bind(instance, method);
        PETSC_CHECK(
            TSMonitorSet(this->ts, invoke_monitor_delegate, &this->monitor_method, nullptr));
    }

    /// Clears all the monitors that have been set on a time-stepping object.
    void monitor_cancel();

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

    /// Set a local rhs function evaluation function
    ///
    /// @tparam T C++ class type
    /// @param instance Instance of class T
    /// @param method Member function in class T
    template <class T>
    void
    set_rhs_function_local(T * instance, void (T::*method)(Real time, const Vector & x, Vector & F))
    {
        this->compute_rhs_local_method.bind(instance, method);
        auto dm = this->problem->get_dm();
        PETSC_CHECK(DMTSSetRHSFunctionLocal(dm,
                                            invoke_compute_rhs_local_delegate,
                                            &this->compute_rhs_local_method));
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

    template <class T>
    void
    set_time_boundary_local(T * instance, void (T::*method)(Real, const Vector &, const Vector &))
    {
        this->compute_boundary_local_method.bind(instance, method);
        auto dm = this->problem->get_dm();
        PETSC_CHECK(DMTSSetBoundaryLocal(dm,
                                         invoke_compute_boundary_delegate,
                                         &this->compute_boundary_local_method));
    }

private:
    /// PETSc TS object
    TS ts;
    /// Method for computing right-hand side
    Delegate<void(Real time, const Vector & x, Vector & F)> compute_rhs_method;
    /// Method for computing right-hand side
    Delegate<void(Real time, const Vector & x, Vector & F)> compute_rhs_local_method;
    /// Method for computing F(t,U,U_t) where F() = 0
    Delegate<void(Real time, const Vector & x, const Vector & x_t, Vector & F)>
        compute_ifunction_local_method;
    /// Method to compute the matrix dF/dU + a*dF/dU_t where F(t,U,U_t) is the function provided by
    /// `set_ifunction_local`
    Delegate<void(Real time,
                  const Vector & X,
                  const Vector & X_t,
                  Real x_t_shift,
                  Matrix & J,
                  Matrix & Jp)>
        compute_ijacobian_local_method;
    /// Method for essential boundary data for a local implicit function evaluation.
    Delegate<void(Real time, const Vector & x, const Vector & x_t)> compute_boundary_local_method;
    /// with set_i Method for monitoring the solve
    Delegate<ErrorCode(Int it, Real rnorm, const Vector & x)> monitor_method;
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
    static ErrorCode invoke_compute_rhs_local_delegate(DM, Real time, Vec x, Vec F, void * ctx);
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
    static ErrorCode
    invoke_compute_boundary_delegate(DM, Real time, Vec x, Vec x_t, void * context);
};

} // namespace godzilla
