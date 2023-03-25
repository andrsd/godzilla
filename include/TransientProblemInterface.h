#pragma once

#include "petscts.h"
#include "Types.h"
#include "Vector.h"

namespace godzilla {

class Problem;
class TimeSteppingAdaptor;

/// Interface for transient simulations
///
class TransientProblemInterface {
public:
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

protected:
    /// Initialize
    virtual void init();
    /// Create
    virtual void create();
    /// Set up monitors
    virtual void set_up_monitors();
    /// Set up time integration scheme
    virtual void set_up_time_scheme() = 0;
    /// Called before the time step solve
    virtual PetscErrorCode pre_step();
    /// Called after the time step is done solving
    virtual PetscErrorCode post_step();
    /// TS monitor callback
    virtual PetscErrorCode ts_monitor_callback(Int stepi, Real time, Vec x);
    /// Check if problem converged
    ///
    /// @return `true` if solve converged, otherwise `false`
    virtual bool converged() const;
    /// Solve
    virtual void solve(Vector & x);

    /// Problem this interface is part of
    Problem * problem;
    /// PETSc TS object
    TS ts;
    /// Time-stepping adaptor
    TimeSteppingAdaptor * ts_adaptor;
    /// Simulation start time
    const Real & start_time;
    /// Simulation end time
    const Real & end_time;
    /// Time step size
    const Real & dt;
    /// Converged reason
    TSConvergedReason converged_reason;

public:
    static Parameters parameters();

    friend PetscErrorCode __transient_pre_step(TS ts);
    friend PetscErrorCode __transient_post_step(TS ts);
    friend PetscErrorCode __transient_monitor(TS ts, Int stepi, Real time, Vec x, void * ctx);
};

} // namespace godzilla
