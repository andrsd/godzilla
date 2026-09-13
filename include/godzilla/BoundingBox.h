// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "godzilla/Span.h"
#include "godzilla/Formatters.h"
#include <array>

namespace godzilla {

template <Dimension D>
class BoundingBox {
public:
    BoundingBox()
    {
        for (Int i = 0; i < D; ++i) {
            this->mn[i] = std::numeric_limits<Real>::max();
            this->mx[i] = std::numeric_limits<Real>::lowest();
        }
    }

    BoundingBox(const std::array<Real, D> & minimum, const std::array<Real, D> & maximum) :
        mn(minimum),
        mx(maximum)
    {
    }

    BoundingBox(Span<const Real> minimum, Span<const Real> maximum)
    {
        expect_true(
            minimum.size() == D,
            fmt::format("Dimension of minimum ({}) does not match boundning box dimension ({})",
                        minimum.size(),
                        D));
        expect_true(
            maximum.size() == D,
            fmt::format("Dimension of maximum ({}) does not match boundning box dimension ({})",
                        maximum.size(),
                        D));

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
