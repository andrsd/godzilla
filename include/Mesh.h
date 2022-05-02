#pragma once

#include "petsc.h"
#include "Common.h"
#include "Object.h"
#include "PrintInterface.h"
#include "Array.h"
#include "Vertex.h"
#include "Element.h"

namespace godzilla {

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

    /// Returns the total number of vertices stored.
    uint get_num_vertices() const;

    /// Returns the total number of elements stored.
    uint get_num_elements() const;

    virtual void create() override;

    /// Get vertex with index `id`
    const Vertex * get_vertex(const Index & id) const;

    /// Get an element using its ID
    ///
    /// @param[in] id ID of an element
    const Element * get_element(const Index & id) const;

    const Array<const Vertex *> &
    get_vertices() const
    {
        return this->vertices;
    }

    const Array<const Element *> &
    get_elements() const
    {
        return this->elements;
    }

    /// Get vertex ID
    ///
    /// @param[in] e Element
    /// @param[in] vertex Local edge number
    virtual PetscInt get_vertex_id(const Element * e, uint vertex) const;

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

    virtual uint get_marker_by_name(const std::string & bnd_name) const;

    /// Set partitioner type
    ///
    /// @param type Type of the partitioner
    virtual void set_partitioner_type(const std::string & type);

    /// Set partitioner type
    ///
    /// @param type Type of the partitioner
    virtual void set_partition_overlap(int overlap);

protected:
    virtual void create_dm() = 0;

    void create_elements();
    void create_vertices();

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
    /// Mesh partitioner
    PetscPartitioner partitioner;
    /// Partition overlap for mesh partitioning
    PetscInt partition_overlap;
    /// Map from boundary name to marker
    std::map<std::string, uint> bnd_name_to_marker;

public:
    static InputParameters valid_params();
};

} // namespace godzilla
