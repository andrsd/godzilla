// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "godzilla/CallStack.h"
#include "godzilla/Validation.h"
#include "godzilla/Assert.h"
#include "fmt/format.h"
#include <numeric>
#include <algorithm>
#include <cmath>

namespace godzilla {

/// User-defined piecewise linear function
///
/// The independent variable 'x' has to be increasing.
/// User have to specify at least 2 points
template <FloatingPoint Y = Real>
class PiecewiseLinear {
public:
    /// Construct an empty linear interpolation object
    PiecewiseLinear() = default;

    /// Construct interpolation object by providing independent and dependent values
    ///
    /// @param x Independent values
    /// @param y Dependent values
    PiecewiseLinear(const std::vector<Real> & x, const std::vector<Y> & y) : x_(x), y_(y)
    {
        CALL_STACK_MSG();
        expect_true(this->x_.size() == this->y_.size(),
                    fmt::format("Size of 'x' ({}) does not match size of 'y' ({})",
                                this->x_.size(),
                                this->y_.size()));
        expect_true(this->x_.size() >= 2,
                    fmt::format("Size of 'x' is {}. It must be 2 or more", this->x_.size()));

        expect_true(not validation::contains_nan<Real>(this->x_), "Parameter 'x' contains a NaN");
        expect_true(not validation::contains_nan<Y>(this->y_), "Parameter 'y' contains a NaN");

        {
            auto [monotonic, index] = validation::check_monotonicity<Real>(this->x_);
            expect_true(
                monotonic,
                fmt::format("Values in 'x' must be increasing. Failed at index '{}'", index));
        }
    }

    /// Sample the interpolation at a point
    ///
    /// @params x Point where we sample the interpolation
    /// @return Interpolated value
    Y
    evaluate(Real x) const
    {
        CALL_STACK_MSG();
        auto sz = this->x_.size();
        expect_true(sz >= 2, "PiecewiseLinear is not initialized");

        if (x < this->x_[0])
            return this->y_[0];
        else if (x > this->x_[sz - 1])
            return this->y_[sz - 1];
        else {
            std::size_t lo_idx = 0;
            std::size_t hi_idx = sz - 1;
            while (true) {
                if (lo_idx + 1 == hi_idx) {
                    return this->y_[lo_idx] +
                           (x - this->x_[lo_idx]) * ((this->y_[hi_idx] - this->y_[lo_idx]) /
                                                     (this->x_[hi_idx] - this->x_[lo_idx]));
                }
                else {
                    auto mid_idx = std::midpoint(lo_idx, hi_idx);
                    if (x < this->x_[mid_idx])
                        hi_idx = mid_idx;
                    else
                        lo_idx = mid_idx;
                }
            }
        }
    }

private:
    /// Independent values
    std::vector<Real> x_;
    /// Dependent values
    std::vector<Y> y_;
};

} // namespace godzilla
