#pragma once

#include "petscdm.h"
#include "Common.h"

namespace godzilla {

/// Base class for elements (abstract)
///
class Element {
public:
    Element(PetscInt id);
    Element(const Element & o);

    /// Return polytope type
    ///
    /// USed for interaction with PETSc
    virtual const DMPolytopeType get_potytope_type() const = 0;

    /// Get element ID
    const PetscInt & get_id() const;

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
    virtual PetscInt get_vertex(uint local) const = 0;

    /// Get vertex indices
    ///
    virtual const PetscInt * get_vertices() const = 0;

    /// Get edge orientation
    ///
    /// @param[in] edge Local edge number
    /// @return Edge orientation
    virtual uint get_edge_orientation(uint edge) const = 0;

    /// Get face orientation
    ///
    /// @param[in] face Local edge number
    /// @return Face orientation
    virtual uint get_face_orientation(uint face) const = 0;

protected:
    /// Element ID
    PetscInt id;

    /// Marker
    uint marker;
};

/// Base class for 1D elements
///
class Element1D : public Element {
public:
    Element1D(PetscInt id) : Element(id) {}
};

/// Base class for 2D elements
///
class Element2D : public Element {
public:
    Element2D(PetscInt id) : Element(id) {}

    virtual uint get_num_edges() const = 0;
};

/// Base class for 3D elements
///
class Element3D : public Element {
public:
    Element3D(PetscInt id) : Element(id) {}

    virtual uint get_num_edges() const = 0;
    virtual uint get_num_faces() const = 0;
};

} // namespace godzilla
