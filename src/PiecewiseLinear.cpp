// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/PiecewiseLinear.h"
#include "godzilla/CallStack.h"
#include "godzilla/Exception.h"
#include "godzilla/Assert.h"
#include "fmt/format.h"

namespace godzilla {

PiecewiseLinear::PiecewiseLinear(const std::vector<Real> & x, const std::vector<Real> & y) :
    x(x),
    y(y)
{
    CALL_STACK_MSG();
    assert_true(this->x.size() == this->y.size(),
                fmt::format("Size of 'x' ({}) does not match size of 'y' ({})",
                            this->x.size(),
                            this->y.size()));
    assert_true(this->x.size() >= 2,
                fmt::format("Size of 'x' is {}. It must be 2 or more", this->x.size()));

    // check monotonicity
    for (std::size_t i = 0; i < this->x.size() - 1; ++i) {
        if (this->x[i] >= this->x[i + 1])
            throw Exception("Values in 'x' must be increasing. Failed at index '{}'", i + 1);
    }
}

void
PiecewiseLinear::create(const std::vector<Real> & x, const std::vector<Real> & y)
{
    CALL_STACK_MSG();
    this->x = x;
    this->y = y;
}

Real
PiecewiseLinear::evaluate(Real x)
{
    CALL_STACK_MSG();
    auto sz = this->x.size();
    if (x < this->x[0])
        return this->y[0];
    else if (x > this->x[sz - 1])
        return this->y[sz - 1];
    else {
        std::size_t lo_idx = 0;
        std::size_t hi_idx = sz - 1;
        while (true) {
            if (lo_idx + 1 == hi_idx) {
                return this->y[lo_idx] + (x - this->x[lo_idx]) *
                                             (this->y[hi_idx] - this->y[lo_idx]) /
                                             (this->x[hi_idx] - this->x[lo_idx]);
            }
            else {
                auto mid_idx = lo_idx + (hi_idx - lo_idx) / 2;
                if (x < this->x[mid_idx])
                    hi_idx = mid_idx;
                else
                    lo_idx = mid_idx;
            }
        }
    }
}

} // namespace godzilla
