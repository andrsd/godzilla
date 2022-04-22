#pragma once

#include "Shapeset1D.h"

namespace godzilla {

/// Shapeset for a reference mapping
///
class RefMapShapesetEdge : public Shapeset1D {
public:
    RefMapShapesetEdge();
    virtual ~RefMapShapesetEdge();

    virtual uint
    get_vertex_index(uint vertex) const
    {
        return this->vertex_indices[vertex];
    }

    virtual uint get_num_edge_fns(uint order) const { return 0; }

    virtual uint get_num_face_fns(uint order) const { return 0; }

    virtual uint *
    get_bubble_indices(uint /*order*/) const
    {
        return nullptr;
    }

    virtual uint
    get_num_bubble_fns(uint /*order*/) const
    {
        return 0;
    }

    virtual uint get_order(uint index) const;

    virtual void
    get_values(uint n, uint index, uint np, const QPoint1D * pt, uint component, Real * vals) const
    {
        for (uint k = 0; k < np; k++)
            vals[k] = this->shape_table[n][component][index](pt[k].x);
    }

    virtual Real
    get_value(uint n, uint index, Real x, uint component) const
    {
        return this->shape_table[n][component][index](x);
    }

protected:
    /// Shape function tables
    shape_fn_1d_t ** shape_table[NUM_VALUE_TYPES];
    /// Indices of vertex shape functions on reference element, indexing: []
    uint * vertex_indices;

public:
    static const RefMapShapesetEdge * get();
};

} // namespace godzilla
