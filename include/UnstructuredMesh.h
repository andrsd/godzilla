#pragma once

#include "Grid.h"
#include "petscpartitioner.h"

namespace godzilla {

/// Base class for unstructured meshes
///
class UnstructuredMesh : public Grid {
public:
    UnstructuredMesh(const InputParameters & parameters);

    /// Set partitioner
    ///
    /// @param partitioner The partitioner
    virtual void setPartitioner(PetscPartitioner partitioner);

    /// Distribute mesh over processes
    ///
    /// @param overlap The overlap of partitions
    virtual void distribute(PetscInt overlap = 0);

public:
    static InputParameters validParams();
};

} // namespace godzilla
