#pragma once

#include "petsc.h"
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
    Mesh(const MPI_Comm & comm = MPI_COMM_WORLD);
    virtual ~Mesh();

    /// Get the MPI comm
    const MPI_Comm & get_comm() const;

    /// Frees all data associated with the mesh.
    void free();

    /// Get the mesh dimension
    const uint & get_dimension() const;

    /// Set the mesh dimension
    void set_dimension(const uint & dim);

    /// Returns the total number of elements stored.
    uint get_num_elements() const;

    /// Create internal structures
    void create();

    uint get_cone_size(const Index & id) const;

    void get_cone(const Index & id, const PetscInt * cone[]) const;

    /// Get type of a cell
    ///
    /// @param cell[in] Cell number (vertex, edge, face, element)
    DMPolytopeType get_cell_type(PetscInt cell) const;

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

    /// Get edge ID
    ///
    /// @param[in] e Element
    /// @param[in] edge Local edge number
    virtual Index get_edge_id(Element * e, uint edge) const;

    /// Get face ID
    ///
    /// @param[in] e Element
    /// @param[in] face Local face number
    virtual Index get_face_id(Element * e, uint face) const;

protected:
    /// MPI communicator
    MPI_Comm comm;
    /// DM for the unstructured mesh
    DM dm;
    /// Spatial dimension
    uint dim;
    /// Mesh vertices
    Array<const Vertex *> vertices;
    /// Mesh elements
    Array<const Element *> elements;
    /// Boundaries
    Array<const SideBoundary *> boundaries;
};

} // namespace godzilla
