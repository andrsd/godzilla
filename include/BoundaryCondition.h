#pragma once

#include "Object.h"
#include "PrintInterface.h"
#include "petsc.h"

namespace godzilla {

/// Base class for boundary conditions
///
class BoundaryCondition : public Object, public PrintInterface {
public:
    /// Constructor for building the object via Factory
    BoundaryCondition(const InputParameters & params);

    /// Get the boundary names this BC is active on
    ///
    /// @return Array of boundary names
    virtual const std::vector<std::string> & getBoundary() const;

    /// Get the ID of the field this boundary condition operates on
    ///
    /// @return ID of the field
    virtual PetscInt getFieldId() const;

    /// Get the number of components of this boundary condition
    ///
    /// @return The number of components
    virtual PetscInt getNumComponents() const = 0;

    /// Get the type of this boundary condition (per component)
    ///
    /// @return Vector of boundary condition types (one per component)
    virtual std::vector<DMBoundaryConditionType> getBcType() const = 0;

protected:
    /// Evaluate the initial condition
    ///
    /// @param dim The spatial dimension
    /// @param time The time at which to sample
    /// @param x The coordinates
    /// @param Nc The number of components
    /// @param u  The output field values
    virtual void
    evaluate(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt Nc, PetscScalar u[]) = 0;

    /// List of boundary names
    const std::vector<std::string> & boundary;

public:
    /// Method for building InputParameters for this class
    static InputParameters validParams();

    friend PetscErrorCode __boundary_condition_function(PetscInt dim,
                                                        PetscReal time,
                                                        const PetscReal x[],
                                                        PetscInt Nc,
                                                        PetscScalar u[],
                                                        void * ctx);
};

/// C callback passed into PETSc that can call the evaluate method on the BoundaryCondition class
PetscErrorCode __boundary_condition_function(PetscInt dim,
                                             PetscReal time,
                                             const PetscReal x[],
                                             PetscInt Nc,
                                             PetscScalar u[],
                                             void * ctx);

} // namespace godzilla
