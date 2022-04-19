#pragma once

#include "Error.h"
#include "Array.h"

namespace godzilla {

/// Quadrature point in 2D
///
struct QPoint2D {
    /// x-coordinate
    Real x;
    /// y-coordinate
    Real y;
    /// weight
    Real w;

    QPoint2D() {}
    QPoint2D(Real x, Real y, Real w)
    {
        this->x = x;
        this->y = y;
        this->w = w;
    }

    Real
    operator[](uint idx) const
    {
        if (idx == 0)
            return this->x;
        else if (idx == 1)
            return this->y;
        else
            error("Index out of bounds");
    }
};

/// Numerical quadratures in 2D
///
class Quadrature2D {
public:
    QPoint2D *
    get_points(uint order) const
    {
        return tables[order];
    }

    inline uint
    get_num_points(uint order) const
    {
        return np[order];
    };

    QPoint2D *
    get_edge_points(uint edge, uint order) const
    {
        return edge_tables[edge][order];
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
    QPoint2D ** tables;
    /// tables with integration points for edges (?)
    /// indexing: [edge][order][point no.]
    QPoint2D *** edge_tables;
};

} // namespace godzilla
