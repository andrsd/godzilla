#pragma once

#include "Object.h"
#include "PrintInterface.h"
#include "petsc.h"

namespace godzilla {

class FEProblemInterface;

/// Base class for auxiliary fields
///
class AuxiliaryField : public Object, public PrintInterface {
public:
    AuxiliaryField(const InputParameters & params);
    virtual ~AuxiliaryField();

    /// Set up the auxiliary field
    ///
    /// @param dm The main DM
    /// @param dm_aux DM for the auxiliary fields
    virtual void set_up(DM dm, DM dm_aux) = 0;

    /// Get the ID of the field this boundary condition operates on
    ///
    /// @return ID of the field
    virtual PetscInt get_field_id() const;

    /// Get the number of constrained components
    ///
    /// @return The number of constrained components
    virtual PetscInt get_num_components() const = 0;

protected:
    /// FE problem this object is part of
    const FEProblemInterface & fepi;

    /// Auxiliary vector
    Vec a;

    /// Block here the auxiliary field lives
    DMLabel block;

public:
    static InputParameters valid_params();
};

} // namespace godzilla
