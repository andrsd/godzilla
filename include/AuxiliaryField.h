#pragma once

#include "Object.h"
#include "PrintInterface.h"
#include "Types.h"
#include "petsc.h"

namespace godzilla {

class FEProblemInterface;

/// Base class for auxiliary fields
///
class AuxiliaryField : public Object, public PrintInterface {
public:
    AuxiliaryField(const InputParameters & params);
    virtual ~AuxiliaryField();

    virtual void create() override;

    virtual DMLabel get_label() const;

    /// Get the ID of the field this boundary condition operates on
    ///
    /// @return ID of the field
    virtual PetscInt get_field_id() const;

    /// Get the number of constrained components
    ///
    /// @return The number of constrained components
    virtual PetscInt get_num_components() const = 0;

    virtual PetscFunc * get_func() const = 0;

protected:
    /// FE problem this object is part of
    const FEProblemInterface * fepi;

    /// Region name this auxiliary field is defined on
    const std::string & region;

    /// Block here the auxiliary field lives
    DMLabel label;

public:
    static InputParameters valid_params();
};

} // namespace godzilla
