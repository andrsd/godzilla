#pragma once

#include "AuxiliaryField.h"
#include "FunctionInterface.h"

namespace godzilla {

/// Auxiliary field set by a function
///
class FunctionAuxiliaryField : public AuxiliaryField, public FunctionInterface {
public:
    FunctionAuxiliaryField(const InputParameters & params);

    virtual void create();
    virtual PetscInt get_num_components() const;
    virtual PetscFunc * get_func() const;
    virtual void
    evaluate(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt nc, PetscScalar u[]);

public:
    static InputParameters valid_params();
};

} // namespace godzilla
