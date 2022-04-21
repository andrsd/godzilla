#pragma once

#include "Common.h"
#include "Array.h"
#include "Vertex.h"
#include "Element.h"

namespace godzilla {

#define FOR_ALL_ELEMENTS(idx, mesh)                                         \
    for (Index(idx) = (mesh)->get_elements().first(); (idx) != INVALID_IDX; \
         (idx) = (mesh)->get_elements().next(idx))

/// Information about a boundary
struct SideBoundary {
    SideBoundary(const Index & eid, const uint side, const uint & marker) :
        elem_id(eid),
        side(side),
        marker(marker)
    {
    }

    /// Element ID
    Index elem_id;
    /// Local element side this boundary is associated with
    uint side;
    /// Marker
    uint marker;
};

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

    /// Get an element using its ID
    ///
    /// @param[in] id ID of an element
    const Element * get_element(const Index & id) const;

    /// Set a boundary
    ///
    /// @param[in] eid Element ID
    /// @param[in] local_side Local element side
    /// @param[in] marker Marker associated with the boundary
    void set_boundary(const Index & eid, const uint & local_side, const uint & marker);

    const Array<const Element *> &
    get_elements() const
    {
        return this->elements;
    }

    const Array<const SideBoundary *> &
    get_boundaries() const
    {
        return this->boundaries;
    }

protected:
    /// Mesh vertices
    Array<const Vertex *> vertices;
    /// Mesh elements
    Array<const Element *> elements;
    /// Boundaries
    Array<const SideBoundary *> boundaries;
};

} // namespace godzilla
