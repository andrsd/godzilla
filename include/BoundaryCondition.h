#pragma once

#include "Object.h"
#include "PrintInterface.h"
#include "petsc.h"

namespace godzilla {

/// Base class for boundary conditions
///
class BoundaryCondition : public Object,
                          public PrintInterface
{
public:
    BoundaryCondition(const InputParameters & params);

    /// Get the boundary names this BC is active on
    ///
    /// @return Array of boundary names
    virtual const std::vector<std::string> & getBoundary() const;

    virtual PetscInt getFieldId() const;
    virtual PetscInt getNumComponents() const = 0;
    virtual std::vector<DMBoundaryConditionType> getBcType() const = 0;

protected:
    /// Evaluate the initial condition
    ///
    /// @param dim The spatial dimension
    /// @param time The time at which to sample
    /// @param x The coordinates
    /// @param Nc The number of components
    /// @param u  The output field values
    virtual void evaluate(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt Nc, PetscScalar u[]) = 0;

    /// List of boundary names
    const std::vector<std::string> & boundary;

public:
    static InputParameters validParams();

    friend PetscErrorCode __boundary_condition_function(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt Nc, PetscScalar u[], void *ctx);
};

PetscErrorCode __boundary_condition_function(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt Nc, PetscScalar u[], void *ctx);

} // godzilla
