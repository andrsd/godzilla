// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Function.h"
#include "godzilla/Types.h"

namespace godzilla {

class LinearInterpolation;

/// User-defined piecewise linear function
///
/// The independent variable 'x' has to be increasing.
/// User have to specify at least 2 points
class PiecewiseLinear : public Function {
public:
    explicit PiecewiseLinear(const Parameters & params);
    ~PiecewiseLinear() override;

    void register_callback(mu::Parser & parser) override;

    /// Evaluate this function at point 'x'
    Real evaluate(Real x);

private:
    /// Linear interpolation object used for function evaluation
    LinearInterpolation * linpol;

public:
    static Parameters parameters();
};

} // namespace godzilla
