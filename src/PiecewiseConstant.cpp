// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/PiecewiseConstant.h"
#include "godzilla/CallStack.h"
#include "godzilla/Exception.h"
#include "godzilla/Assert.h"

namespace godzilla {

PiecewiseConstant::PiecewiseConstant(Continuity cont,
                                     const std::vector<Real> & x,
                                     const std::vector<Real> & y) :
    continuity(cont),
    x(x),
    y(y)
{
    CALL_STACK_MSG();
    assert_true(this->x.size() + 1 == this->y.size(),
                fmt::format("Size of 'x' ({}) does not match size of 'y' ({}).",
                            this->x.size(),
                            this->y.size()));

    assert_true(!this->x.empty(),
                fmt::format("Size of 'x' is {}. It must be 1 or more.", this->x.size()));

    // check monotonicity
    for (std::size_t i = 0; i < this->x.size() - 1; ++i) {
        if (this->x[i] >= this->x[i + 1])
            throw Exception("Values in 'x' must be increasing. Failed at index '{}'.", i + 1);
    }
}

Real
PiecewiseConstant::evaluate(Real x)
{
    CALL_STACK_MSG();
    switch (this->continuity) {
    case RIGHT:
        return eval_right_cont(x);
    case LEFT:
        return eval_left_cont(x);
    default:
        throw InternalError("Unknown continuity type");
    }
}

Real
PiecewiseConstant::eval_right_cont(Real x)
{
    CALL_STACK_MSG();
    std::size_t sz = this->x.size();
    if (x < this->x[0])
        return this->y[0];
    else if (x >= this->x[sz - 1])
        return this->y[sz];
    else {
        std::size_t lo_idx = 0;
        std::size_t hi_idx = sz;
        while (true) {
            if (lo_idx + 1 == hi_idx) {
                return this->y[lo_idx + 1];
            }
            else {
                std::size_t mid_idx = (lo_idx + hi_idx) / 2;
                if (x < this->x[mid_idx])
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
    std::size_t sz = this->x.size();
    if (x <= this->x[0])
        return this->y[0];
    else if (x > this->x[sz - 1])
        return this->y[sz];
    else {
        std::size_t lo_idx = 0;
        std::size_t hi_idx = sz - 1;
        while (true) {
            if (lo_idx + 1 == hi_idx) {
                return this->y[lo_idx + 1];
            }
            else {
                std::size_t mid_idx = (lo_idx + hi_idx) / 2;
                if (x <= this->x[mid_idx])
                    hi_idx = mid_idx;
                else
                    lo_idx = mid_idx;
            }
        }
    }
}

} // namespace godzilla
