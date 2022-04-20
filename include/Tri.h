#pragma once

#include "Common.h"
#include "Element.h"

namespace godzilla {

/// Represents a triangle
///
class Tri : public Element2D {
public:
    Tri();
    Tri(Index v[]);
    Tri(Index v1, Index v2, Index v3);
    Tri(const Tri & o);

    virtual uint get_num_vertices() const { return NUM_VERTICES; }
    virtual uint get_num_edges() const { return NUM_EDGES; }

public:
    static const uint NUM_VERTICES = 3;
    static const uint NUM_EDGES = 3;

protected:
    /// Array of vertex indices that build up the triangle
    Index vertices[NUM_VERTICES];
};

} // namespace godzilla
