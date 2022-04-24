#pragma once

#include "Common.h"
#include "Element.h"

namespace godzilla {

/// Represents an edge
///
class Edge : public Element1D {
public:
    Edge(PetscInt id);
    Edge(PetscInt id, PetscInt v[]);
    Edge(PetscInt id, PetscInt v1, PetscInt v2);
    Edge(const Edge & o);

    virtual const DMPolytopeType
    get_potytope_type() const
    {
        return DM_POLYTOPE_SEGMENT;
    }
    virtual uint
    get_num_vertices() const
    {
        return NUM_VERTICES;
    }
    virtual PetscInt
    get_vertex(uint local) const
    {
        return this->vertices[local];
    }
    virtual const PetscInt *
    get_vertices() const
    {
        return &(this->vertices[0]);
    }

    virtual uint get_edge_orientation(uint edge) const;
    virtual uint get_face_orientation(uint face) const;

public:
    static const uint NUM_VERTICES = 2;

protected:
    /// Array of vertex indices that build up the edge
    PetscInt vertices[NUM_VERTICES];
};

} // namespace godzilla
