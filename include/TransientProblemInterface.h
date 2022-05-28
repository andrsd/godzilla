#pragma once

#include "petscts.h"

namespace godzilla {

class Problem;

/// Interface for transient simulations
///
class TransientProblemInterface {
public:
    TransientProblemInterface(Problem * problem, const InputParameters & params);
    virtual ~TransientProblemInterface();

protected:
    /// Initialize
    virtual void init();
    /// Create
    virtual void create();
    /// Set up monitors
    virtual void set_up_monitors();
    /// Set up time integration scheme
    virtual void set_up_time_scheme();
    /// Called before the time step solve
    virtual PetscErrorCode pre_step();
    /// Called after the time step is done solving
    virtual PetscErrorCode post_step();
    /// TS monitor callback
    virtual PetscErrorCode ts_monitor_callback(PetscInt stepi, PetscReal time, Vec x);
    /// Solve
    virtual void solve(Vec x);

    /// Problem this interface is part of
    Problem * problem;
    /// PETSc TS object
    TS ts;
    /// Simulation start time
    const PetscReal & start_time;
    /// Simulation end time
    const PetscReal & end_time;
    /// Time step size
    const PetscReal & dt;
    /// Number of the time step
    PetscInt step_num;

public:
    static InputParameters valid_params();

    friend PetscErrorCode __transient_pre_step(TS ts);
    friend PetscErrorCode __transient_post_step(TS ts);
    friend PetscErrorCode
    __transient_monitor(TS ts, PetscInt stepi, PetscReal time, Vec x, void * ctx);
};

} // namespace godzilla
