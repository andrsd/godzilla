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

    /// Set partitioner type
    ///
    /// @param type Type of the partitioner
    virtual void set_partitioner_type(const std::string & type);

    /// Set partitioner type
    ///
    /// @param type Type of the partitioner
    virtual void set_partition_overlap(PetscInt overlap);

    /// Output partitioning
    ///
    /// @param viewer PetscViewer to store the partitioning
    virtual void output_partitioning(PetscViewer viewer);

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
