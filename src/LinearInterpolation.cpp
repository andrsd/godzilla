// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/CallStack.h"
#include "godzilla/LinearInterpolation.h"
#include "fmt/printf.h"

namespace godzilla {

LinearInterpolation::LinearInterpolation() : x(), y()
{
    CALL_STACK_MSG();
}

LinearInterpolation::LinearInterpolation(const std::vector<Real> & ax,
                                         const std::vector<Real> & ay) :
    x(ax),
    y(ay)
{
    CALL_STACK_MSG();
    if (this->x.size() != this->y.size())
        throw std::domain_error(fmt::format("Size of 'x' ({}) does not match size of 'y' ({}).",
                                            this->x.size(),
                                            this->y.size()));

    if (this->x.size() < 2)
        throw std::domain_error(
            fmt::format("Size of 'x' is {}. It must be 2 or more.", this->x.size()));
    else {
        // check monotonicity
        for (std::size_t i = 0; i < this->x.size() - 1; i++) {
            if (this->x[i] >= this->x[i + 1])
                throw std::domain_error(
                    fmt::format("Values in 'x' must be increasing. Failed at index '{}'.", i + 1));
        }
    }
}

void
LinearInterpolation::create(const std::vector<Real> & x, const std::vector<Real> & y)
{
    CALL_STACK_MSG();
    this->x = x;
    this->y = y;
}

Real
LinearInterpolation::sample(Real x)
{
    CALL_STACK_MSG();
    std::size_t sz = this->x.size();
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
                std::size_t mid_idx = (lo_idx + hi_idx) / 2;
                if (x < this->x[mid_idx])
                    hi_idx = mid_idx;
                else
                    lo_idx = mid_idx;
            }
        }
    }
}

} // namespace godzilla
