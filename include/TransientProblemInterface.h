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
    ///
    /// @param comm MPI communicator
    virtual void init();
    /// Create
    virtual void create();
    /// Called before the time step solve
    virtual void set_up_time_scheme();
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
};

} // namespace godzilla
