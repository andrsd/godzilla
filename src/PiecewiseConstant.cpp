// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/PiecewiseConstant.h"
#include "godzilla/CallStack.h"
#include "godzilla/Exception.h"
#include "godzilla/Assert.h"
#include "godzilla/Utils.h"

namespace godzilla {

PiecewiseConstant::PiecewiseConstant() : continuity_(LEFT) {}

PiecewiseConstant::PiecewiseConstant(Continuity cont,
                                     const std::vector<Real> & x,
                                     const std::vector<Real> & y) :
    continuity_(cont),
    x_(x),
    y_(y)
{
    CALL_STACK_MSG();
    expect_true(this->x_.size() + 1 == this->y_.size(),
                fmt::format("'x' (size={}) must have one more entry than 'y' (size={})",
                            this->x_.size(),
                            this->y_.size()));

    expect_true(!this->x_.empty(),
                fmt::format("Size of 'x' is {}. It must be 1 or more", this->x_.size()));

    // check monotonicity
    for (std::size_t i = 0; i < this->x_.size() - 1; ++i) {
        if (this->x_[i] >= this->x_[i + 1])
            throw Exception(
                fmt::format("Values in 'x' must be increasing - failed at index '{}'", i + 1));
    }
}

auto
PiecewiseConstant::get_continuity() const -> Continuity
{
    return this->continuity_;
}

Real
PiecewiseConstant::evaluate(Real x)
{
    CALL_STACK_MSG();
    switch (this->continuity_) {
    case RIGHT:
        return eval_right_cont(x);
    case LEFT:
        return eval_left_cont(x);
    }
    utils::unreachable();
}

Real
PiecewiseConstant::eval_right_cont(Real x)
{
    CALL_STACK_MSG();
    std::size_t sz = this->x_.size();
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
                std::size_t mid_idx = (lo_idx + hi_idx) / 2;
                if (x < this->x_[mid_idx])
                    hi_idx = mid_idx;
                else
                    lo_idx = mid_idx;
            }
        }
    }
}

Real
PiecewiseConstant::eval_left_cont(Real x)
{
    CALL_STACK_MSG();
    std::size_t sz = this->x_.size();
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
                std::size_t mid_idx = (lo_idx + hi_idx) / 2;
                if (x <= this->x_[mid_idx])
                    hi_idx = mid_idx;
                else
                    lo_idx = mid_idx;
            }
        }
    }
}

} // namespace godzilla
