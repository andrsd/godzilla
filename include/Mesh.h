#pragma once

#include "GodzillaConfig.h"
#include "Array.h"
#include "Vertex.h"
#include "Element.h"

namespace godzilla {

/// Represents the geometry
///
class Mesh {
public:
    Mesh();
    virtual ~Mesh();
    /// Frees all data associated with the mesh.
    void free();

    /// Returns the total number of elements stored.
    uint getNumElements() const;

    void setVertex(const Index & id, const Vertex * vertex);

    void setElement(const Index & id, const Element * elem);

protected:
    /// Mesh vertices
    Array<const Vertex *> vertices;
    /// Mesh elements
    Array<const Element *> elements;
};

} // namespace godzilla
