#pragma once

#include "Function.h"
#include "LinearInterpolation.h"

namespace godzilla {

/// User-defined piecewise linear function
///
/// The independent variable 'x' has to be increasing.
/// User have to specify at least 2 points
class PiecewiseLinear : public Function {
public:
    explicit PiecewiseLinear(const Parameters & params);

    void check() override;

    void register_callback(mu::Parser & parser) override;

    /// Evaluate this function at point 'x'
    Real evaluate(Real x);

protected:
    /// Linear interpolation object used for function evaluation
    LinearInterpolation linpol;

public:
    static Parameters parameters();
};

} // namespace godzilla
