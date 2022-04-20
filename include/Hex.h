#pragma once

#include "Common.h"
#include "Element.h"

namespace godzilla {

/// Represents hexahedron in 3D
///
class Hex : public Element3D {
public:
    Hex();
    Hex(Index v[]);
    Hex(Index v1, Index v2, Index v3, Index v4, Index v5, Index v6, Index v7, Index v8);
    Hex(const Hex & o);

    virtual uint get_num_vertices() const { return NUM_VERTICES; }
    virtual uint get_num_edges() const { return NUM_EDGES; }
    virtual uint get_num_faces() const { return NUM_FACES; }

public:
    static const int NUM_VERTICES = 8;
    static const int NUM_FACES = 6;
    static const int NUM_EDGES = 12;

protected:
    /// Array of vertex indices that build up the hexahedron
    Index vertices[NUM_VERTICES];
};

} // namespace godzilla
