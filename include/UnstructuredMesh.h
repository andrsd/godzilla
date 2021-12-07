#pragma once

#include "Grid.h"

namespace godzilla {

/// Base class for unstructured meshes
///
class UnstructuredMesh : public Grid {
public:
    UnstructuredMesh(const InputParameters & parameters);

    /// Distribute mesh over processes
    ///
    /// @param overlap The overlap of partitions
    virtual void distribute(PetscInt overlap = 0);

public:
    static InputParameters validParams();
};

} // namespace godzilla
