#pragma once

#include "Common.h"
#include "Element.h"

namespace godzilla {

/// Represents a quadrilateral
///
class Quad : public Element2D {
public:
    Quad();
    Quad(Index v[]);
    Quad(Index v1, Index v2, Index v3, Index v4);
    Quad(const Quad & o);

    virtual uint get_num_vertices() const { return NUM_VERTICES; }
    virtual uint get_num_edges() const { return NUM_EDGES; }

public:
    static const uint NUM_VERTICES = 4;
    static const uint NUM_EDGES = 4;

protected:
    /// Array of vertex indices that build up the triangle
    Index vertices[NUM_VERTICES];
};

} // namespace godzilla
