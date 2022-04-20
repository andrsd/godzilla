#pragma once

#include "Shapeset1D.h"

namespace godzilla {

/// H1 shapeset for a 1D edge element
///
class H1LobattoShapesetEdge : public Shapeset1D {
public:
    H1LobattoShapesetEdge();
    virtual ~H1LobattoShapesetEdge();

    virtual uint
    get_vertex_index(uint vertex) const
    {
        return this->vertex_indices[vertex];
    }

    virtual uint *
    get_bubble_indices(uint order)
    {
        return this->bubble_indices[order];
    }

    virtual uint
    get_num_bubble_fns(uint order) const
    {
        return (order > 1) ? (order - 1) : 0;
    }

    virtual uint get_order(uint index) const;

    virtual void
    get_values(uint n, uint index, uint np, const QPoint1D * pt, uint component, Real * vals)
    {
        for (uint k = 0; k < np; k++)
            vals[k] = this->shape_table[n][component][index](pt[k].x);
    }

    virtual Real
    get_value(uint n, uint index, Real x, uint component)
    {
        return this->shape_table[n][component][index](x);
    }

protected:
    shape_fn_1d_t ** shape_table[NUM_VALUE_TYPES];
    /// Indices of vertex shape functions on reference element, indexing: []
    uint * vertex_indices;
    /// Indices of bubble functions on reference element, indexing: [order][]
    uint ** bubble_indices;
    /// Number of bubble functions on reference element, indexing: [order]
    uint * bubble_count;
};

} // namespace godzilla
