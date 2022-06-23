#pragma once

#include "Mesh.h"
#include "petscpartitioner.h"

namespace godzilla {

/// Base class for unstructured meshes
///
class UnstructuredMesh : public Mesh {
public:
    UnstructuredMesh(const InputParameters & parameters);
    virtual ~UnstructuredMesh();

    virtual void create() override;

    /// Return the number of mesh vertices
    virtual PetscInt get_num_vertices() const;

    /// Get range of vertex indices
    ///
    /// @param first First vertex index
    /// @param last Last vertex index plus one
    void get_vertex_idx_range(PetscInt & first, PetscInt & last) const;

    /// Return the number of mesh elements
    virtual PetscInt get_num_elements() const;

    /// Get range of element indices
    ///
    /// @param first First element index
    /// @param last Last element index plus one
    void get_element_idx_range(PetscInt & first, PetscInt & last) const;

    /// Set partitioner type
    ///
    /// @param type Type of the partitioner
    virtual void set_partitioner_type(const std::string & type);

    /// Set partitioner type
    ///
    /// @param type Type of the partitioner
    virtual void set_partition_overlap(PetscInt overlap);

    /// Is the first cell in the mesh a simplex?
    ///
    /// @return true if cell is a simplex, otherwise false
    virtual bool is_simplex() const;

    /// Get cell set name
    ///
    /// @param id The ID of the cell set
    /// @return Cell set name
    const std::string & get_cell_set_name(PetscInt id) const;

    /// Get face set name
    ///
    /// @param id The id of the face set
    /// @return Facet name
    const std::string & get_face_set_name(PetscInt id) const;

protected:
    virtual void distribute() override;

    void create_cell_set(PetscInt id, const std::string & name);

    void create_face_set(PetscInt id, const std::string & name);

    /// Mesh partitioner
    PetscPartitioner partitioner;

    /// Partition overlap for mesh partitioning
    PetscInt partition_overlap;

    /// Cell set names
    std::map<PetscInt, std::string> cell_set_names;

    /// Face set names
    std::map<PetscInt, std::string> face_set_names;

public:
    static InputParameters valid_params();
};

} // namespace godzilla
