#pragma once

#include "GodzillaConfig.h"
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
    virtual void setup_time_scheme();
    /// Solve
    virtual void solve(Vec x);

    /// PETSc TS object
    TS ts;
    /// Simulation start time
    const Real & start_time;
    /// Simulation end time
    const Real & end_time;
    /// Time step size
    const Real & dt;

public:
    static InputParameters validParams();
};

} // namespace godzilla
