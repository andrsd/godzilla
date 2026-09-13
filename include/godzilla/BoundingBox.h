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

    bool
    contains(Span<const Real> x, Real tol) const
    {
        CALL_STACK_MSG();

        GODZILLA_ASSERT_TRUE(
            x.size() == D,
            fmt::format("Point dimension ({}) does not match bounding box dimension ({})",
                        s.size(),
                        D));

        for (Int d = 0; d < D; d++) {
            if (x[d] + tol < this->mn[d] || this->mx[d] + tol < x[d])
                return false;
        }
        return true;
    }

private:
    std::array<Real, D> mn;
    std::array<Real, D> mx;

public:
    static BoundingBox
    create_from_points(Span<const Real> coords)
    {
        CALL_STACK_MSG();

        expect_true(
            coords.size() % D == 0,
            fmt::format("'coords' must be divisible by {}, it has {} entries", D, coords.size()));

        BoundingBox bbox;
        for (Int d = 0; d < D; d++) {
            bbox.mn[d] = PETSC_MAX_REAL;
            bbox.mx[d] = PETSC_MIN_REAL;
        }
        auto n_points = coords.size() / D;
        for (Int i = 0; i < n_points; i++) {
            for (Int d = 0; d < D; d++) {
                bbox.mn[d] = std::min(bbox.mn[d], coords[i * D + d]);
                bbox.mx[d] = std::max(bbox.mx[d], coords[i * D + d]);
            }
        }
        return bbox;
    }
};

} // namespace godzilla
