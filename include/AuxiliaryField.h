#pragma once

#include "GodzillaConfig.h"
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
    explicit AuxiliaryField(const Parameters & params);

    void create() override;

    NO_DISCARD virtual DMLabel get_label() const;

    NO_DISCARD virtual const std::string & get_region() const;

    /// Get the ID of the field this boundary condition operates on
    ///
    /// @return ID of the field
    NO_DISCARD virtual PetscInt get_field_id() const;

    /// Get the number of constrained components
    ///
    /// @return The number of constrained components
    NO_DISCARD virtual PetscInt get_num_components() const = 0;

    NO_DISCARD virtual PetscFunc * get_func() const = 0;

    virtual void * get_context();

protected:
    /// FE problem this object is part of
    const FEProblemInterface * fepi;

    /// Field name
    const std::string & field;

    /// Region name this auxiliary field is defined on
    const std::string & region;

    /// Field ID
    PetscInt fid;

    /// Block here the auxiliary field lives
    DMLabel label;

public:
    static Parameters parameters();
};

} // namespace godzilla
