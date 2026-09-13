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
            this->min[i] = std::numeric_limits<Real>::max();
            this->max[i] = std::numeric_limits<Real>::lowest();
        }
    }

    BoundingBox(const std::array<Real, D> & minimum, const std::array<Real, D> & maximum) :
        min(minimum),
        max(maximum)
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
            this->min[i] = minimum[i];
            this->max[i] = maximum[i];
        }
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
            if (x[d] + tol < this->min[d] || this->max[d] + tol < x[d])
                return false;
        }
        return true;
    }

    std::array<Real, D> min;
    std::array<Real, D> max;

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
            bbox.min[d] = PETSC_MAX_REAL;
            bbox.max[d] = PETSC_MIN_REAL;
        }
        auto n_points = coords.size() / D;
        for (Int i = 0; i < n_points; i++) {
            for (Int d = 0; d < D; d++) {
                bbox.min[d] = std::min(bbox.min[d], coords[i * D + d]);
                bbox.max[d] = std::max(bbox.max[d], coords[i * D + d]);
            }
        }
        return bbox;
    }
};

template <Dimension DIM>
bool
intersect(const BoundingBox<DIM> & a, const BoundingBox<DIM> & b, Real tol)
{
    for (Int d = 0; d < DIM; d++) {
        if (a.max[d] + tol < b.min[d] || b.max[d] + tol < a.min[d])
            return false;
    }
    return true;
}

} // namespace godzilla

namespace mpicpp_lite {

template <godzilla::Dimension D>
struct DatatypeTraits<godzilla::BoundingBox<D>> {
    static MPI_Datatype
    get()
    {
        std::vector<MPI_Datatype> types = { mpicpp_lite::mpi_datatype<godzilla::Real>(),
                                            mpicpp_lite::mpi_datatype<godzilla::Real>() };
        std::vector<int> blk_lens = { D, D };
        std::vector<MPI_Aint> offsets = { offsetof(godzilla::BoundingBox<D>, min),
                                          offsetof(godzilla::BoundingBox<D>, max) };
        return type_create_struct(types, blk_lens, offsets);
    }
};

} // namespace mpicpp_lite
