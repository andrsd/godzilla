#pragma once

#include "Object.h"
#include "PrintInterface.h"
#include "petsc.h"

namespace godzilla {

class DiscreteProblemInterface;

/// Base class for boundary conditions
///
class BoundaryCondition : public Object, public PrintInterface {
public:
    BoundaryCondition(const InputParameters & params);

    virtual void create() override;

    /// Get the boundary name this BC is active on
    ///
    /// @return The boundary name
    virtual const std::string & get_boundary() const;

    /// Get the ID of the field this boundary condition operates on
    ///
    /// @return ID of the field
    virtual PetscInt get_field_id() const;

    /// Get the number of constrained components
    ///
    /// @return The number of constrained components
    virtual PetscInt get_num_components() const = 0;

    /// Get the type of this boundary condition
    ///
    /// @return Type of boundary condition
    virtual DMBoundaryConditionType get_bc_type() const = 0;

    /// Get the component numbers this boundary condition is constraining
    ///
    /// @return Vector of component numbers
    virtual std::vector<PetscInt> get_components() const = 0;

    /// Set up this boundary condition
    virtual void set_up();

protected:
    /// Set up the PETSc callback
    virtual void set_up_callback() = 0;

    /// Discrete problem this object is part of
    const DiscreteProblemInterface * dpi;

    /// DM object
    DM dm;

    /// DS object
    PetscDS ds;

    /// DMLabel associated with the boundary name this boundary condition acts on
    DMLabel label;

    /// Field ID this boundary condition is attached to
    PetscInt fid;

    /// Number of IDs in the label
    PetscInt n_ids;

    /// IDs of the label
    const PetscInt * ids;

    /// List of boundary names
    const std::string & boundary;

public:
    /// Method for building InputParameters for this class
    static InputParameters valid_params();
};

} // namespace godzilla
