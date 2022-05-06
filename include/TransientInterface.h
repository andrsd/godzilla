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
    virtual void setUpTimeScheme();
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
    /// Number of the time step
    PetscInt step_num;

public:
    static InputParameters validParams();
};

} // namespace godzilla
