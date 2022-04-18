#pragma once

#include "GodzillaConfig.h"
#include "Element.h"

namespace godzilla {

/// Represents tetrahedron
///
class Tetra : public Element {
public:
    Tetra();
    Tetra(Index v[]);
    Tetra(Index v1, Index v2, Index v3, Index v4);
    Tetra(const Tetra & o);

    virtual uint get_num_vertices() const;
    virtual uint get_num_edges() const;
    virtual uint get_num_faces() const;

public:
    static const uint NUM_VERTICES = 4;
    static const uint NUM_FACES = 4;
    static const uint NUM_EDGES = 6;

protected:
    /// Array of vertex indices that build up the tetrahedron
    Index vertices[NUM_VERTICES];
};

} // namespace godzilla
