#pragma once

#include "Types.h"
#include "AuxiliaryField.h"

namespace godzilla {

/// Constant auxiliary field
///
class ConstantAuxiliaryField : public AuxiliaryField {
public:
    explicit ConstantAuxiliaryField(const Parameters & params);

    void create() override;
    PetscInt get_num_components() const override;
    PetscFunc * get_func() const override;
    void evaluate(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt nc, PetscScalar u[]);

protected:
    const std::vector<PetscReal> & values;
    unsigned int num_comps;

public:
    static Parameters parameters();
};

} // namespace godzilla
