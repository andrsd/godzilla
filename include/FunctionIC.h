#pragma once

#include "InitialCondition.h"
#include "FunctionInterface.h"

namespace godzilla {

/// Initial condition given by a function expression
///
class FunctionIC : public InitialCondition, public FunctionInterface {
public:
    explicit FunctionIC(const Parameters & params);

    void create() override;
    Int get_num_components() const override;

    void evaluate(Int dim, PetscReal time, const PetscReal x[], Int Nc, PetscScalar u[]) override;

public:
    static Parameters parameters();
};

} // namespace godzilla
