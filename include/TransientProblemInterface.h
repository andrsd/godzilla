#pragma once

#include "petscts.h"

namespace godzilla {

class Problem;
class TimeSteppingAdaptor;

/// Interface for transient simulations
///
class TransientProblemInterface {
public:
    TransientProblemInterface(Problem * problem, const InputParameters & params);
    virtual ~TransientProblemInterface();

    /// Set time stepping adaptivity using an adaptor class
    ///
    /// @param adaptor Time stepping adaptor object
    void set_time_stepping_adaptor(TimeSteppingAdaptor * adaptor);

    /// Set time stepping adaptivity using a type
    ///
    /// @param type Time stepping scheme
    void set_time_stepping_scheme(TSAdaptType type);

    /// Get TSAdapt object
    TSAdapt get_ts_adapt() const;

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
    virtual PetscErrorCode ts_monitor_callback(PetscInt stepi, PetscReal time, Vec x);
    /// TS adapt callback
    virtual PetscErrorCode ts_adapt_choose(PetscReal h,
                                           PetscInt * next_sc,
                                           PetscReal * next_h,
                                           PetscBool * accept,
                                           PetscReal * wlte,
                                           PetscReal * wltea,
                                           PetscReal * wlter);
    /// Check if problem converged
    ///
    /// @return `true` if solve converged, otherwise `false`
    virtual bool converged() const;
    /// Solve
    virtual void solve(Vec x);

    /// Problem this interface is part of
    Problem * problem;
    /// PETSc TS object
    TS ts;
    /// TSAdapt object
    TSAdapt ts_adapt;
    /// Time-stepping adaptor
    TimeSteppingAdaptor * ts_adaptor;
    /// Simulation start time
    const PetscReal & start_time;
    /// Simulation end time
    const PetscReal & end_time;
    /// Time step size
    const PetscReal & dt;
    /// Converged reason
    TSConvergedReason converged_reason;

public:
    static InputParameters valid_params();

    static void register_types();

    friend PetscErrorCode __transient_pre_step(TS ts);
    friend PetscErrorCode __transient_post_step(TS ts);
    friend PetscErrorCode
    __transient_monitor(TS ts, PetscInt stepi, PetscReal time, Vec x, void * ctx);
    friend PetscErrorCode __ts_adapt_choose(TSAdapt adapt,
                                            TS ts,
                                            PetscReal h,
                                            PetscInt * next_sc,
                                            PetscReal * next_h,
                                            PetscBool * accept,
                                            PetscReal * wlte,
                                            PetscReal * wltea,
                                            PetscReal * wlter);
};

} // namespace godzilla
