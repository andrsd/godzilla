#pragma once

#include "InitialCondition.h"
#include "FunctionInterface.h"

namespace godzilla {

/// Initial condition given by a function expression
///
class FunctionIC : public InitialCondition, public FunctionInterface {
public:
    FunctionIC(const InputParameters & params);

    virtual void create() override;
    virtual PetscInt get_num_components() const override;

    virtual void evaluate(PetscInt dim,
                          PetscReal time,
                          const PetscReal x[],
                          PetscInt Nc,
                          PetscScalar u[]) override;

public:
    static InputParameters valid_params();
};

} // namespace godzilla
