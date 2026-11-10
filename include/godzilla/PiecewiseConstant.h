// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "godzilla/CallStack.h"
#include "godzilla/Validation.h"
#include "godzilla/Assert.h"
#include "godzilla/Utils.h"
#include <numeric>

namespace godzilla {

/// User-defined piecewise constant function
///
/// The independent variable 'x' has to be increasing.
/// User have to specify at least 1 point
template <FloatingPoint Y = Real>
class PiecewiseConstant {
public:
    PiecewiseConstant() : continuity_(Continuity::LEFT) {}

    /// Construct a piecewise constant function
    ///
    /// @param cont Left or right continuity
    /// @param x Independent variable
    /// @param y Dependent variable
    PiecewiseConstant(Continuity cont, const std::vector<Real> & x, const std::vector<Y> & y) :
        continuity_(cont),
        x_(x),
        y_(y)
    {
        CALL_STACK_MSG();
        expect_true(this->x_.size() + 1 == this->y_.size(),
                    fmt::format("'y' (size={}) must have one more entry than 'x' (size={})",
                                this->y_.size(),
                                this->x_.size()));

        expect_true(!this->x_.empty(),
                    fmt::format("Size of 'x' is {}. It must be 1 or more", this->x_.size()));

        expect_true(not validation::contains_nan<Real>(this->x_), "Parameter 'x' contains a NaN");
        expect_true(not validation::contains_nan<Y>(this->y_), "Parameter 'y' contains a NaN");

        {
            auto [monotonic, index] = validation::check_monotonicity<Real>(this->x_);
            expect_true(
                monotonic,
                fmt::format("Values in 'x' must be increasing. Failed at index '{}'", index));
        }
    }

    /// Get type of continuity
    ///
    /// @return Continuity type
    Continuity
    get_continuity() const
    {
        return this->continuity_;
    }

    /// Evaluate this function at point 'x'
    Y
    evaluate(Real x)
    {
        CALL_STACK_MSG();
        expect_true(not this->x_.empty(), "PiecewiseConstant is not initialized");

        switch (this->continuity_) {
        case Continuity::RIGHT:
            return eval_right_cont(x);
        case Continuity::LEFT:
            return eval_left_cont(x);
        }
        utils::unreachable();
    }

private:
    Y
    eval_right_cont(Real x)
    {
        CALL_STACK_MSG();
        auto sz = this->x_.size();
        if (x < this->x_[0])
            return this->y_[0];
        else if (x >= this->x_[sz - 1])
            return this->y_[sz];
        else {
            std::size_t lo_idx = 0;
            std::size_t hi_idx = sz;
            while (true) {
                if (lo_idx + 1 == hi_idx) {
                    return this->y_[lo_idx + 1];
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

    Y
    eval_left_cont(Real x)
    {
        CALL_STACK_MSG();
        auto sz = this->x_.size();
        if (x <= this->x_[0])
            return this->y_[0];
        else if (x > this->x_[sz - 1])
            return this->y_[sz];
        else {
            std::size_t lo_idx = 0;
            std::size_t hi_idx = sz - 1;
            while (true) {
                if (lo_idx + 1 == hi_idx) {
                    return this->y_[lo_idx + 1];
                }
                else {
                    auto mid_idx = std::midpoint(lo_idx, hi_idx);
                    if (x <= this->x_[mid_idx])
                        hi_idx = mid_idx;
                    else
                        lo_idx = mid_idx;
                }
            }
        }
    }

    Continuity continuity_;
    /// Independent values
    std::vector<Real> x_;
    /// Dependent values
    std::vector<Y> y_;
};

} // namespace godzilla
