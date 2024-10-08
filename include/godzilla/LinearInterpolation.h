// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include <vector>

namespace godzilla {

/// Utility class for linear interpolation
///
class LinearInterpolation {
public:
    /// Construct an empty linear interpolation object
    LinearInterpolation();
    /// Construct interpolation object by providing independent and dependent values
    ///
    /// @param x Independent values
    /// @param y Dependent values
    LinearInterpolation(const std::vector<Real> & x, const std::vector<Real> & y);

    /// Create the interpolation object by providing independent and dependent values
    ///
    /// @param x Independent values
    /// @param y Dependent values
    void create(const std::vector<Real> & x, const std::vector<Real> & y);

    /// Sample the interpolation at a point
    ///
    /// @params x Point where we sample the interpolation
    /// @return Interpolated value
    Real sample(Real x);

private:
    /// Independent values
    std::vector<Real> x;
    /// Dependent values
    std::vector<Real> y;
};

} // namespace godzilla
