#pragma once

#include "petscts.h"

namespace godzilla {

/// Interface for transient simulations
///
class TransientInterface {
public:
    TransientInterface(const InputParameters & params);
    virtual ~TransientInterface();

protected:
    /// Initialize
    ///
    /// @param comm MPI communicator
    virtual void init(const MPI_Comm & comm);
    /// Create
    virtual void create(DM dm);
    /// Called before the time step solve
    virtual void setupTimeScheme();
    /// Called before the time step solve
    virtual PetscErrorCode onPreStep();
    /// Called after the time step is done solving
    virtual PetscErrorCode onPostStep();
    /// TS monitor callback
    virtual PetscErrorCode tsMonitorCallback(PetscInt stepi, PetscReal time, Vec X);
    /// Setup monitors
    virtual void setupMonitors();
    /// Solve
    virtual void solve(Vec x);

    /// PETSc TS object
    TS ts;
    /// Simulation start time
    const PetscReal & start_time;
    /// Simulation end time
    const PetscReal & end_time;
    /// Time step size
    const PetscReal & dt;

public:
    static InputParameters validParams();

    friend PetscErrorCode __transient_pre_step(TS ts);
    friend PetscErrorCode __transient_post_step(TS ts);
    friend PetscErrorCode
    __transient_monitor(TS ts, PetscInt stepi, PetscReal time, Vec X, void * ctx);
};

} // namespace godzilla
