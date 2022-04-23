#pragma once

#include "petsc.h"
#include "Common.h"
#include "Object.h"
#include "PrintInterface.h"
#include "Array.h"
#include "Vertex.h"
#include "Element.h"

namespace godzilla {

#define FOR_ALL_ELEMENTS(idx, mesh)                                         \
    for (Index(idx) = (mesh)->get_elements().first(); (idx) != INVALID_IDX; \
         (idx) = (mesh)->get_elements().next(idx))

#define FOR_EACH_SIDE_BOUNDARY(idx, mesh)                                     \
    for (Index(idx) = (mesh)->get_side_boundaries().first(); (idx) != INVALID_IDX; \
         (idx) = (mesh)->get_side_boundaries().next(idx))

/// Information about a side boundary (element, local side)
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

/// Unstructured mesh
///
/// TODO: rename this to UnstructuredMesh
class Mesh : public Object, public PrintInterface {
public:
    Mesh(const InputParameters & parameters);
    virtual ~Mesh();

    /// Get the DM object
    DM get_dm() const;

    /// Frees all data associated with the mesh.
    void free();

    /// Get the mesh dimension
    const int & get_dimension() const;

    /// Set the mesh dimension
    void set_dimension(const int & dim);

    /// Returns the total number of elements stored.
    uint get_num_elements() const;

    virtual void create() override;

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

    /// Get an side boundary using its ID
    ///
    /// @param[in] idx Index returned by iterating over Array returned by get_side_boundaries()
    const SideBoundary * get_side_boundary(const Index & idx) const;

    const Array<const Element *> &
    get_elements() const
    {
        return this->elements;
    }

    const Array<const SideBoundary *> &
    get_side_boundaries() const
    {
        return this->side_boundaries;
    }

    /// Get vertex ID
    ///
    /// @param[in] e Element
    /// @param[in] vertex Local edge number
    virtual Index get_vertex_id(const Element * e, uint vertex) const;

    /// Get edge ID
    ///
    /// @param[in] e Element
    /// @param[in] edge Local edge number
    virtual Index get_edge_id(const Element * e, uint edge) const;

    /// Get face ID
    ///
    /// @param[in] e Element
    /// @param[in] face Local face number
    virtual Index get_face_id(const Element * e, uint face) const;

    /// Set partitioner type
    ///
    /// @param type Type of the partitioner
    virtual void set_partitioner_type(const std::string & type);

    /// Set partitioner type
    ///
    /// @param type Type of the partitioner
    virtual void set_partition_overlap(int overlap);

protected:
    uint get_cone_size(const Index & id) const;

    void get_cone(const Index & id, const PetscInt * cone[]) const;

    /// Get type of a cell
    ///
    /// @param cell[in] Cell number (vertex, edge, face, element)
    DMPolytopeType get_cell_type(PetscInt cell) const;

    /// Distribute mesh over processes
    virtual void distribute();

    /// DM for the unstructured mesh
    DM dm;
    /// Spatial dimension
    int dim;
    /// Mesh vertices
    Array<const Vertex *> vertices;
    /// Mesh elements
    Array<const Element *> elements;
    /// Side boundaries
    Array<const SideBoundary *> side_boundaries;
    /// Mesh partitioner
    PetscPartitioner partitioner;
    /// Partition overlap for mesh partitioning
    PetscInt partition_overlap;

public:
    static InputParameters validParams();
};

} // namespace godzilla
