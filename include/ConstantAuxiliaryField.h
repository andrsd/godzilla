#pragma once

#include "Types.h"
#include "AuxiliaryField.h"

namespace godzilla {

/// Constant auxiliary field
///
class ConstantAuxiliaryField : public AuxiliaryField {
public:
    ConstantAuxiliaryField(const Parameters & params);

    virtual void create();
    virtual PetscInt get_num_components() const;
    virtual PetscFunc * get_func() const;
    virtual void
    evaluate(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt nc, PetscScalar u[]);

protected:
    const std::vector<PetscReal> & values;
    unsigned int num_comps;

public:
    static Parameters parameters();
};

} // namespace godzilla
