#pragma once

#include "Common.h"
#include "Element.h"

namespace godzilla {

/// Represents an edge
///
class Edge : public Element {
public:
    Edge();
    Edge(Index v[]);
    Edge(Index v1, Index v2);
    Edge(const Edge & o);

public:
    static const uint NUM_VERTICES = 2;

protected:
    /// Array of vertex indices that build up the edge
    Index vertices[NUM_VERTICES];
};

} // namespace godzilla
