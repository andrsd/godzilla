// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Function.h"
#include "godzilla/Types.h"

namespace godzilla {

/// Constant function
class ConstantFunction : public Function {
public:
    explicit ConstantFunction(const Parameters & params);

    void create() override;

    void register_callback(mu::Parser & parser) override;

    /// Evaluate this function at point 'x'
    Real evaluate(Real x);

private:
    /// Constant value
    const Real & val;

public:
    static Parameters parameters();
};

} // namespace godzilla
