#pragma once

#include "Grid.h"
#include "petscpartitioner.h"

namespace godzilla {

/// Base class for unstructured meshes
///
class UnstructuredMesh : public Grid {
public:
    UnstructuredMesh(const InputParameters & parameters);
    virtual ~UnstructuredMesh();

    /// Set partitioner type
    ///
    /// @param type Type of the partitioner
    virtual void setPartitionerType(const std::string & type);

    /// Set partitioner type
    ///
    /// @param type Type of the partitioner
    virtual void setPartitionOverlap(PetscInt overlap);

    /// Output partitioning
    ///
    /// @param viewer PetscViewer to store the partitioning
    virtual void outputPartitioning(PetscViewer viewer);

protected:
    virtual void distribute() override;

    /// Mesh partitioner
    PetscPartitioner partitioner;

    /// Partition overlap for mesh partitioning
    PetscInt partition_overlap;

public:
    static InputParameters validParams();
};

} // namespace godzilla
