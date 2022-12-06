#pragma once

#include "GodzillaConfig.h"
#include "Object.h"
#include "PrintInterface.h"
#include "petsc.h"

namespace godzilla {

class DiscreteProblemInterface;

/// Base class for boundary conditions
///
class BoundaryCondition : public Object, public PrintInterface {
public:
    explicit BoundaryCondition(const Parameters & params);
    ~BoundaryCondition() override = default;

    void create() override;

    /// Get the boundary name this BC is active on
    ///
    /// @return The boundary name
    NO_DISCARD virtual const std::string & get_boundary() const;

    /// Get the ID of the field this boundary condition operates on
    ///
    /// @return ID of the field
    NO_DISCARD virtual PetscInt get_field_id() const;

    /// Get the component numbers this boundary condition is constraining
    ///
    /// @return Vector of component numbers
    NO_DISCARD virtual const std::vector<PetscInt> & get_components() const = 0;

    /// Get DiscreteProblemInterface
    ///
    /// @return Discrete problem this BC is part of
    NO_DISCARD virtual const DiscreteProblemInterface * get_discrete_problem_interface() const;

    /// Set up this boundary condition
    virtual void set_up();

protected:
    /// Add boundary to the discretization object
    virtual void add_boundary() = 0;

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

    /// IDs of the label
    std::vector<PetscInt> ids;

    /// List of boundary names
    const std::string & boundary;

public:
    /// Method for building Parameters for this class
    static Parameters parameters();
};

} // namespace godzilla
