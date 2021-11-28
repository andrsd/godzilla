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

    /// Get the boundary name this BC is active on
    ///
    /// @return The boundary name
    virtual const std::string & getBoundary() const;

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

    /// Set up this boundary condition
    ///
    /// @param dm DM of problem (should have PetscDS)
    virtual void setUp(DM dm);

protected:
    /// Set up the PETSc callback
    virtual void setUpCallback() = 0;

    /// DM object
    DM dm;

    /// DS object
    PetscDS ds;

    /// DMLabel associated with the boundary name this boundary condition acts on
    DMLabel label;

    /// Number of IDs in the label
    PetscInt n_ids;

    /// IDs of the label
    const PetscInt * ids;

    /// List of boundary names
    const std::string & boundary;

public:
    /// Method for building InputParameters for this class
    static InputParameters validParams();
};

} // namespace godzilla
