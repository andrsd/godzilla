#pragma once

#include "Shapeset3D.h"

namespace godzilla {

class H1LobattoShapesetTetra : public Shapeset3D {
public:
    H1LobattoShapesetTetra();
    virtual ~H1LobattoShapesetTetra();

    virtual uint
    get_vertex_index(uint vertex) const
    {
        return this->vertex_indices[vertex];
    }

    virtual uint *
    get_edge_indices(uint edge, uint ori, uint order)
    {
        return this->edge_indices[edge][ori];
    }

    virtual uint *
    get_face_indices(uint face, uint ori, uint order)
    {
        return this->face_indices[face][ori];
    }

    virtual uint *
    get_bubble_indices(uint order)
    {
        return this->bubble_indices[order];
    }

    virtual uint
    get_num_edge_fns(uint order) const
    {
        return this->edge_count[order];
    }

    virtual uint
    get_num_face_fns(uint order) const
    {
        return this->face_count[order];
    }

    virtual uint
    get_num_bubble_fns(uint order) const
    {
        return this->bubble_count[order];
    }

    virtual uint get_order(uint index) const;

    virtual void
    get_values(EValueType n, uint index, uint np, QPoint3D * pt, uint component, Real * vals)
    {
        for (int k = 0; k < np; k++)
            vals[k] = this->shape_table[n][component][index](pt[k].x, pt[k].y, pt[k].z);
    }

    virtual Real
    get_value(EValueType n, uint index, Real x, Real y, Real z, uint component)
    {
        return this->shape_table[n][component][index](x, y, z);
    }

protected:
    shape_fn_3d_t ** shape_table[NUM_VALUE_TYPES];

    /// Indices of vertex shape functions on reference element, indexing: []
    uint * vertex_indices;
    /// Indices of edge shape functions on reference element, indexing: [edge index][ori][]
    uint *** edge_indices;
    /// Indices of face shape functions on reference element, indexing: [face index][ori][]
    uint *** face_indices;
    /// Indices of bubble functions on reference element, indexing: [order][]
    uint ** bubble_indices;

    /// Number of edge shape functions on reference element, indexing: [order]
    uint * edge_count;
    /// Number of face shape functions on reference element, indexing: [order]
    uint * face_count;
    /// Number of bubble functions on reference element, indexing: [order]
    uint * bubble_count;
};

} // namespace godzilla
