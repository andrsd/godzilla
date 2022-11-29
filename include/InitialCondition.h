#pragma once

#include "GodzillaConfig.h"
#include "Object.h"
#include "PrintInterface.h"
#include "Types.h"

namespace godzilla {

class DiscreteProblemInterface;

/// Base class for initial conditions
///
class InitialCondition : public Object, public PrintInterface {
public:
    explicit InitialCondition(const Parameters & params);

    void create() override;
    NO_DISCARD virtual PetscInt get_field_id() const;
    NO_DISCARD virtual PetscInt get_num_components() const = 0;

    /// Get pointer to the C function that will be passed into PETSc API
    virtual PetscFunc * get_function();

    /// Get the pointer to the context that will be passed into PETSc API
    virtual void * get_context();

    /// Evaluate the initial condition
    ///
    /// @param dim The spatial dimension
    /// @param time The time at which to sample
    /// @param x The coordinates
    /// @param Nc The number of components
    /// @param u  The output field values
    virtual void
    evaluate(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt Nc, PetscScalar u[]) = 0;

protected:
    /// Discrete problem this object is part of
    const DiscreteProblemInterface * dpi;

    /// Field ID this initial condition is attached to
    PetscInt fid;

public:
    static Parameters parameters();
};

} // namespace godzilla
