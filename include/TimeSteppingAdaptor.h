#pragma once

#include "Object.h"

namespace godzilla {

class Problem;
class TransientProblemInterface;

/// Base class for adaptive time stepping
///
class TimeSteppingAdaptor : public Object {
public:
    TimeSteppingAdaptor(const InputParameters & params);

    /// Choose the next time step size
    ///
    /// @param h Current time steps size
    /// @param next_sc Next time stepping scheme.
    /// @param next_h Next time step size
    /// @param wlte Weighted local truncation error
    /// @param wltea Weighted absolute local truncation error
    /// @param wlter Weighted relative local truncation error
    virtual void choose(PetscReal h,
                        PetscInt * next_sc,
                        PetscReal * next_h,
                        PetscBool * accept,
                        PetscReal * wlte,
                        PetscReal * wltea,
                        PetscReal * wlter) = 0;

protected:
    /// Problem this adaptor is part of
    const Problem * problem;

    /// Transient problem interface this adaptor is part of
    const TransientProblemInterface * tpi;

public:
    static InputParameters valid_params();
};

} // namespace godzilla
