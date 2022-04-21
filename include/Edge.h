#pragma once

#include "Common.h"
#include "Element.h"

namespace godzilla {

/// Represents an edge
///
class Edge : public Element1D {
public:
    Edge();
    Edge(Index v[]);
    Edge(Index v1, Index v2);
    Edge(const Edge & o);

    virtual const DMPolytopeType get_potytope_type() const { return DM_POLYTOPE_SEGMENT; }
    virtual uint get_num_vertices() const { return NUM_VERTICES; }
    virtual Index get_vertex(uint local) const { return this->vertices[local]; }
    virtual const Index * get_vertices() const { return &(this->vertices[0]); }

public:
    static const uint NUM_VERTICES = 2;

protected:
    /// Array of vertex indices that build up the edge
    Index vertices[NUM_VERTICES];
};

} // namespace godzilla
