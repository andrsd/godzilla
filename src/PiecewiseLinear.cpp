// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/PiecewiseLinear.h"
#include "godzilla/CallStack.h"
#include "godzilla/Exception.h"
#include "godzilla/Assert.h"
#include "fmt/format.h"

namespace godzilla {

PiecewiseLinear::PiecewiseLinear(const std::vector<Real> & x, const std::vector<Real> & y) :
    x_(x),
    y_(y)
{
    CALL_STACK_MSG();
    expect_true(this->x_.size() == this->y_.size(),
                fmt::format("Size of 'x' ({}) does not match size of 'y' ({})",
                            this->x_.size(),
                            this->y_.size()));
    expect_true(this->x_.size() >= 2,
                fmt::format("Size of 'x' is {}. It must be 2 or more", this->x_.size()));

    // check monotonicity
    for (std::size_t i = 0; i < this->x_.size() - 1; ++i) {
        if (this->x_[i] >= this->x_[i + 1])
            throw Exception(
                fmt::format("Values in 'x' must be increasing. Failed at index '{}'", i + 1));
    }
}

void
PiecewiseLinear::create(const std::vector<Real> & x, const std::vector<Real> & y)
{
    CALL_STACK_MSG();
    this->x_ = x;
    this->y_ = y;
}

Real
PiecewiseLinear::evaluate(Real x)
{
    CALL_STACK_MSG();
    auto sz = this->x_.size();
    if (x < this->x_[0])
        return this->y_[0];
    else if (x > this->x_[sz - 1])
        return this->y_[sz - 1];
    else {
        std::size_t lo_idx = 0;
        std::size_t hi_idx = sz - 1;
        while (true) {
            if (lo_idx + 1 == hi_idx) {
                return this->y_[lo_idx] + (x - this->x_[lo_idx]) *
                                              (this->y_[hi_idx] - this->y_[lo_idx]) /
                                              (this->x_[hi_idx] - this->x_[lo_idx]);
            }
            else {
                auto mid_idx = lo_idx + (hi_idx - lo_idx) / 2;
                if (x < this->x_[mid_idx])
                    hi_idx = mid_idx;
                else
                    lo_idx = mid_idx;
            }
        }
    }
}

} // namespace godzilla
