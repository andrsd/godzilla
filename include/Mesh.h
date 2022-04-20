#pragma once

#include "Common.h"
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
    uint get_num_elements() const;

    void set_vertex(const Index & id, const Vertex * vertex);

    /// Get vertex with index `id`
    const Vertex * get_vertex(const Index & id) const;

    void set_element(const Index & id, const Element * elem);

protected:
    /// Mesh vertices
    Array<const Vertex *> vertices;
    /// Mesh elements
    Array<const Element *> elements;
};

} // namespace godzilla
