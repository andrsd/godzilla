#pragma once

#include "Object.h"
#include "GPrintInterface.h"

namespace godzilla {

/// Base class for initial conditions
///
class InitialCondition : public Object,
                         public GPrintInterface
{
public:
    InitialCondition(const InputParameters & params);

    virtual PetscInt getFieldId() const;
    virtual PetscInt getNumComponents() const = 0;

protected:
    /// Evaluate the initial condition
    ///
    /// @param dim The spatial dimension
    /// @param time The time at which to sample
    /// @param x The coordinates
    /// @param Nc The number of components
    /// @param u  The output field values
    virtual void evaluate(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt Nc, PetscScalar u[]) = 0;

public:
    static InputParameters validParams();

    friend PetscErrorCode __initial_condition_function(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt Nc, PetscScalar u[], void *ctx);
};

PetscErrorCode __initial_condition_function(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt Nc, PetscScalar u[], void *ctx);

} // godzilla
