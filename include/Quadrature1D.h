#pragma once

#include "Error.h"
#include "Array.h"

namespace godzilla {

/// Quadrature point in 1D
///
struct QPoint1D {
    /// x-coordinate
    Real x;
    /// weight
    Real w;

    QPoint1D(Real x, Real w)
    {
        this->x = x;
        this->w = w;
    }

    Real
    operator[](uint idx) const
    {
        if (idx == 0)
            return this->x;
        else
            error("Index out of bounds");
    }
};

/// Numerical quadrature in 1D
///
class Quadrature1D {
public:
    QPoint1D *
    get_points(uint order) const
    {
        return tables[order];
    }

    uint
    get_num_points(uint order) const
    {
        return np[order];
    }

    uint
    get_max_order() const
    {
        return max_order;
    }

protected:
    /// maximal order for integration
    uint max_order;
    /// number of integration points
    /// indexing: [order]
    uint * np;
    /// tables with integration points
    /// indexing: [order][point no.]
    QPoint1D ** tables;
};

} // namespace godzilla
