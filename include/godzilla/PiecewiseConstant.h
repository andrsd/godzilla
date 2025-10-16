// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Function.h"
#include "godzilla/Types.h"

namespace godzilla {

/// User-defined piecewise constant function
///
/// The independent variable 'x' has to be increasing.
/// User have to specify at least 1 point
class PiecewiseConstant : public Function {
public:
    explicit PiecewiseConstant(const Parameters & params);

    void create() override;

    void register_callback(mu::Parser & parser) override;

    /// Evaluate this function at point 'x'
    Real evaluate(Real x);

protected:
    Real eval_right_cont(Real x);
    Real eval_left_cont(Real x);

private:
    enum Continuity { LEFT, RIGHT } continuity;
    /// Independent values
    const std::vector<Real> x;
    /// Dependent values
    const std::vector<Real> y;

public:
    static Parameters parameters();
};

} // namespace godzilla
