#pragma once

#include "GodzillaConfig.h"
#include "Object.h"
#include "PrintInterface.h"
#include "Label.h"
#include "Types.h"

namespace godzilla {

class DiscreteProblemInterface;

/// Base class for auxiliary fields
///
class AuxiliaryField : public Object, public PrintInterface {
public:
    explicit AuxiliaryField(const Parameters & params);

    void create() override;

    NO_DISCARD virtual Int get_block_id() const;

    NO_DISCARD virtual const Label & get_label() const;

    NO_DISCARD virtual const std::string & get_region() const;

    /// Get the ID of the field this boundary condition operates on
    ///
    /// @return ID of the field
    NO_DISCARD virtual Int get_field_id() const;

    /// Get the number of constrained components
    ///
    /// @return The number of constrained components
    NO_DISCARD virtual Int get_num_components() const = 0;

    NO_DISCARD virtual PetscFunc * get_func() const = 0;

    virtual void * get_context();

    virtual void evaluate(Int dim, Real time, const Real x[], Int nc, Scalar u[]) = 0;

protected:
    /// FE problem this object is part of
    const DiscreteProblemInterface * dpi;

    /// Field name
    const std::string & field;

    /// Region name this auxiliary field is defined on
    const std::string & region;

    /// Field ID
    Int fid;

    /// Block here the auxiliary field lives
    Label label;

    /// Block ID associated with the label where this field is defined
    Int block_id;

public:
    static Parameters parameters();
};

} // namespace godzilla
