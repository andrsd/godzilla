// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/InitialCondition.h"
#include "godzilla/FunctionInterface.h"

namespace godzilla {

/// Initial condition given by a function expression
///
class FunctionInitialCondition : public InitialCondition, protected FunctionInterface {
public:
    explicit FunctionInitialCondition(const Parameters & pars);

    void create() override;
    Int get_num_components() const override;

    void evaluate(Real time, const Real x[], Scalar u[]) override;

public:
    static Parameters parameters();
};

} // namespace godzilla
