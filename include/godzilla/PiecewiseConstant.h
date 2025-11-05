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

    PiecewiseConstant();

    /// Construct a piecewise constant function
    ///
    /// @param cont Left or right continuity
    /// @param x Independent variable
    /// @param y Dependent variable
    PiecewiseConstant(Continuity cont, const std::vector<Real> & x, const std::vector<Real> & y);

    /// Get type of continuity
    ///
    /// @return Continuity type
    Continuity get_continuity() const;

    /// Evaluate this function at point 'x'
    Real evaluate(Real x);

private:
    Real eval_right_cont(Real x);
    Real eval_left_cont(Real x);

    Continuity continuity;
    /// Independent values
    std::vector<Real> x;
    /// Dependent values
    std::vector<Real> y;
};

} // namespace godzilla
