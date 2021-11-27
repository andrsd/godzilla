#pragma once

#include "Object.h"
#include "PrintInterface.h"
#include "petsc.h"

namespace godzilla {

/// Base class for boundary conditions
///
class BoundaryCondition : public Object, public PrintInterface {
public:
    BoundaryCondition(const InputParameters & params);

    /// Get the boundary names this BC is active on
    ///
    /// @return Array of boundary names
    virtual const std::vector<std::string> & getBoundary() const;

    /// Get the ID of the field this boundary condition operates on
    ///
    /// @return ID of the field
    virtual PetscInt getFieldID() const = 0;

    /// Get the number of constrained components
    ///
    /// @return The number of constrained components
    virtual PetscInt getNumComponents() const = 0;

    /// Get the type of this boundary condition
    ///
    /// @return Type of boundary condition
    virtual DMBoundaryConditionType getBcType() const = 0;

    /// Get the component numbers this boundary condition is constraining
    ///
    /// @return Vector of component numbers
    virtual std::vector<PetscInt> getComponents() const = 0;

    /// Evaluate the boundary condition
    ///
    /// @param dim The spatial dimension
    /// @param time The time at which to sample
    /// @param x The coordinates
    /// @param Nc The number of components
    /// @param u  The output field values
    virtual void
    evaluate(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt Nc, PetscScalar u[]) = 0;

    /// Set up this boundary condition
    ///
    /// @param dm DM of problem (should have PetscDS)
    virtual void setUp(DM dm);

protected:
    /// List of boundary names
    const std::vector<std::string> & boundary;

public:
    /// Method for building InputParameters for this class
    static InputParameters validParams();
};

} // namespace godzilla
