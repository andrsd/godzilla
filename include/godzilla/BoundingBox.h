// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include <array>

namespace godzilla {

template <Dimension D>
class BoundingBox {
public:
    BoundingBox()
    {
        for (Int i = 0; i < D; ++i) {
            this->mn[i] = 0.;
            this->mx[i] = 0.;
        }
    }

    BoundingBox(const std::array<Real, D> & minimum, const std::array<Real, D> & maximum) :
        mn(minimum),
        mx(maximum)
    {
    }

    BoundingBox(const Real minimum[], const Real maximum[])
    {
        for (Int i = 0; i < D; ++i) {
            this->mn[i] = minimum[i];
            this->mx[i] = maximum[i];
        }
    }

    std::array<Real, D>
    min() const
    {
        return this->mn;
    }

    std::array<Real, D>
    max() const
    {
        return this->mx;
    }

private:
    std::array<Real, D> mn;
    std::array<Real, D> mx;
};

} // namespace godzilla
