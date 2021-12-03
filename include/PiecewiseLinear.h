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
    PiecewiseLinear(const InputParameters & params);

    virtual void registerCallback(mu::Parser & parser);

    /// Evaluate this function at point 'x'
    PetscReal evaluate(PetscReal x);

protected:
    /// Linear interpolation object used for function evaluation
    LinearInterpolation linpol;

public:
    static InputParameters validParams();
};

} // namespace godzilla
