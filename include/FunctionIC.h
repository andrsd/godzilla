#pragma once

#include "InitialCondition.h"
#include "FunctionInterface.h"

namespace godzilla {

/// Initial condition given by a function expression
///
class FunctionIC : public InitialCondition, public FunctionInterface {
public:
    FunctionIC(const InputParameters & params);

    virtual PetscInt getNumComponents() const;

protected:
    virtual void
    evaluate(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt Nc, PetscScalar u[]);

public:
    static InputParameters validParams();
};

} // namespace godzilla
