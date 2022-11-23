#pragma once

#include "Object.h"

namespace godzilla {

class Problem;
class TransientProblemInterface;

/// Base class for adaptive time stepping
///
class TimeSteppingAdaptor : public Object {
public:
    explicit TimeSteppingAdaptor(const Parameters & params);

    void create() override;

    /// Get TSAdapt object
    ///
    /// @return PETSc TSAdapt object
    TSAdapt get_ts_adapt() const;

    /// Get minimum time step size
    ///
    /// @return Minimum time step size
    PetscReal get_dt_min() const;

    /// Get maximum time step size
    ///
    /// @return Maximum time step size
    PetscReal get_dt_max() const;

protected:
    /// Set the type of time stepping adaptivity
    virtual void set_type() = 0;

    /// Problem this adaptor is part of
    const Problem * problem;

    /// Transient problem interface this adaptor is part of
    const TransientProblemInterface * tpi;

    /// TSAdapt object
    TSAdapt ts_adapt;

    /// Minimum time step
    const PetscReal & dt_min;

    /// Maximum time step
    const PetscReal & dt_max;

public:
    static Parameters parameters();
};

} // namespace godzilla
