#pragma once

#include "GodzillaConfig.h"

namespace godzilla {

/// Base class for elements (abstract)
///
class Element {
public:
    Element();
    Element(const Element & o);
    /// Create a copy of this element
    virtual Element * copy() = 0;

    /// Get element ID
    const Index & get_id() const;
    /// Get element marker
    const uint & get_marker() const;

    /// Get number of vertices
    virtual uint get_num_vertices() const = 0;
    /// Get number of edges
    virtual uint get_num_edges() const = 0;
    /// Get number of faces
    virtual uint get_num_faces() const = 0;

protected:
    /// Element ID
    Index id;
    /// Marker
    uint marker;
};

} // namespace godzilla
