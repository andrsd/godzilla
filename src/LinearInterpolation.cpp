#include "CallStack.h"
#include "LinearInterpolation.h"
#include "Error.h"

namespace godzilla {

LinearInterpolation::LinearInterpolation() : x(), y()
{
    _F_;
}

LinearInterpolation::LinearInterpolation(const std::vector<PetscReal> & ax,
                                         const std::vector<PetscReal> & ay) :
    x(ax),
    y(ay)
{
    _F_;
    check();
}

void
LinearInterpolation::create(const std::vector<PetscReal> & x, const std::vector<PetscReal> & y)
{
    _F_;
    this->x = x;
    this->y = y;
    check();
}

void
LinearInterpolation::check()
{
    _F_;
    if (this->x.size() != this->y.size())
        godzilla::error("LinearInterpolation: size of 'x' (",
                        this->x.size(),
                        ") does not match size of 'y' (",
                        this->y.size(),
                        ")");

    if (this->x.size() < 2)
        godzilla::error("LinearInterpolation: Size of 'x' is ",
                        this->x.size(),
                        ". It must be 2 or more.");
    else {
        // check monotonicity
        PetscReal a = this->x[0];
        for (std::size_t i = 1; i < this->x.size(); i++) {
            if (this->x[i] <= a)
                godzilla::error(
                    "LinearInterpolation: Values in 'x' must be increasing. Failed at index '",
                    i,
                    "'.");
        }
    }
}

PetscReal
LinearInterpolation::sample(PetscReal x)
{
    _F_;
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
