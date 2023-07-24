#pragma once

#include "GodzillaConfig.h"
#include "Types.h"
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
    NO_DISCARD TSAdapt get_ts_adapt() const;

    /// Get minimum time step size
    ///
    /// @return Minimum time step size
    NO_DISCARD Real get_dt_min() const;

    /// Get maximum time step size
    ///
    /// @return Maximum time step size
    NO_DISCARD Real get_dt_max() const;

protected:
    /// Set the type of time stepping adaptivity
    virtual void set_type() = 0;

    /// Problem this adaptor is part of
    Problem * problem;

    /// Transient problem interface this adaptor is part of
    const TransientProblemInterface * tpi;

    /// TSAdapt object
    TSAdapt ts_adapt;

    /// Minimum time step
    const Real & dt_min;

    /// Maximum time step
    const Real & dt_max;

public:
    static Parameters parameters();
};

} // namespace godzilla
