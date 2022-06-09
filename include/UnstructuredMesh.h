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

    /// Return the number of mesh vertices
    virtual PetscInt get_num_vertices() const;

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

protected:
    virtual void distribute() override;

    /// Mesh partitioner
    PetscPartitioner partitioner;

    /// Partition overlap for mesh partitioning
    PetscInt partition_overlap;

public:
    static InputParameters valid_params();
};

} // namespace godzilla
