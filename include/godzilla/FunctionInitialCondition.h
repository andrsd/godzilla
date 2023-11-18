#pragma once

#include "godzilla/InitialCondition.h"
#include "godzilla/FunctionInterface.h"

namespace godzilla {

/// Initial condition given by a function expression
///
class FunctionInitialCondition : public InitialCondition, protected FunctionInterface {
public:
    explicit FunctionInitialCondition(const Parameters & params);

    void create() override;
    Int get_num_components() const override;

    void evaluate(Int dim, Real time, const Real x[], Int Nc, Scalar u[]) override;

public:
    static Parameters parameters();
};

} // namespace godzilla
