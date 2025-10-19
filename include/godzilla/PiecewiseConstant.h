// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"

namespace godzilla {

/// User-defined piecewise constant function
///
/// The independent variable 'x' has to be increasing.
/// User have to specify at least 1 point
class PiecewiseConstant {
public:
    enum Continuity {
        /// left
        LEFT,
        RIGHT
    };

    /// Construct a piecewise constant function
    ///
    /// @param cont Left or right continuity
    /// @param x Independent variable
    /// @param y Dependent variable
    PiecewiseConstant(Continuity cont, const std::vector<Real> & x, const std::vector<Real> & y);

    /// Evaluate this function at point 'x'
    Real evaluate(Real x);

private:
    Real eval_right_cont(Real x);
    Real eval_left_cont(Real x);

    Continuity continuity;
    /// Independent values
    const std::vector<Real> x;
    /// Dependent values
    const std::vector<Real> y;
};

} // namespace godzilla
