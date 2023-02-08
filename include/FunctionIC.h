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

    void evaluate(Int dim, Real time, const Real x[], Int Nc, Scalar u[]) override;

public:
    static Parameters parameters();
};

} // namespace godzilla
