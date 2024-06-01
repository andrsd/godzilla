// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "petscts.h"
#include "godzilla/Types.h"
#include "godzilla/Vector.h"
#include "godzilla/SNESolver.h"
#include "godzilla/TSDelegates.h"

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
    TimeSteppingAdaptor * get_time_stepping_adaptor() const;

    /// Get TS object
    ///
    /// @return PETSc TS object
    TS get_ts() const;

    /// Get the name of time stepping scheme
    const std::string & get_scheme() const;

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
    void set_converged_reason(TSConvergedReason reason);

    /// Gets the reason the time iteration was stopped
    ///
    /// NOTE: Can only be called after the call to solve() is complete.
    ///
    /// @return Converged reason
    TSConvergedReason get_converged_reason() const;

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
    /// @return PETSc error code
    virtual PetscErrorCode compute_rhs(Real time, const Vector & x, Vector & F);

protected:
    /// Get underlying non-linear solver
    SNESolver get_snes() const;
    /// Get time
    Real get_time() const;
    /// Get step number
    Int get_step_number() const;
    /// Initialize
    virtual void init();
    /// Create
    virtual void create();
    /// Set up monitors
    virtual void set_up_monitors();
    /// Set up time integration scheme
    virtual void set_up_time_scheme() = 0;
    /// TS monitor
    PetscErrorCode monitor(Int stepi, Real time, const Vector & x);
    /// Check if problem converged
    ///
    /// @return `true` if solve converged, otherwise `false`
    virtual bool converged() const;
    /// Solve
    virtual void solve(Vector & x);
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
    monitor_set(T * instance, PetscErrorCode (T::*method)(Int, Real, const Vector &))
    {
        this->monitor_method = new internal::TSMonitorMethod<T>(instance, method);
        PETSC_CHECK(TSMonitorSet(this->ts, monitor, this->monitor_method, monitor_destroy));
    }

private:
    /// PETSc TS object
    TS ts;
    /// Method for monitoring the solve
    internal::TSMonitorMethodAbstract * monitor_method;
    /// Time stepping scheme
    const std::string & scheme;
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
    static PetscErrorCode monitor(TS ts, Int stepi, Real time, Vec x, void * ctx);
    static PetscErrorCode monitor_destroy(void ** ctx);
};

} // namespace godzilla
