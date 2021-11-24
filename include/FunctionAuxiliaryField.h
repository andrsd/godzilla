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
    virtual void setUp(DM dm, DM dm_aux);
    virtual PetscInt getNumComponents() const;
    virtual void
    evaluate(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt nc, PetscScalar u[]);

public:
    static InputParameters validParams();
};

} // namespace godzilla
