#pragma once

#include "petscdm.h"
#include "Common.h"

namespace godzilla {

/// Base class for elements (abstract)
///
class Element {
public:
    Element();
    Element(const Element & o);

    /// Return polytope type
    ///
    /// USed for interaction with PETSc
    virtual const DMPolytopeType get_potytope_type() const = 0;

    /// Get element ID
    const Index & get_id() const;

    /// Get element marker
    const uint & get_marker() const;

    /// Set element marker
    void set_marker(const uint & marker);

    /// Get number of element vertices
    virtual uint get_num_vertices() const = 0;

    /// Get vertex index (use that to get the actual vertex from Mesh)
    ///
    /// @param[in] local Local vertex number
    /// @return Vertex index of a given local vertex number
    virtual Index get_vertex(uint local) const = 0;

    /// Get vertex indices
    ///
    virtual const Index * get_vertices() const = 0;

protected:
    /// Element ID
    Index id;

    /// Marker
    uint marker;
};

/// Base class for 1D elements
///
class Element1D : public Element {
public:
};

/// Base class for 2D elements
///
class Element2D : public Element {
public:
    virtual uint get_num_edges() const = 0;
};

/// Base class for 3D elements
///
class Element3D : public Element {
public:
    virtual uint get_num_edges() const = 0;
    virtual uint get_num_faces() const = 0;
};

} // namespace godzilla
