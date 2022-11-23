#pragma once

#include "Object.h"
#include "PrintInterface.h"
#include "petscdm.h"

namespace godzilla {

/// Base class for meshes
///
class Mesh : public Object, public PrintInterface {
public:
    explicit Mesh(const Parameters & parameters);

    virtual DM get_dm() const = 0;

    /// Get the mesh spatial dimension
    ///
    /// @return Mesh spatial dimension
    PetscInt get_dimension() const;

    /// Distribute mesh over processes
    virtual void distribute() = 0;

protected:
    /// Spatial dimension of the mesh
    PetscInt dim;

public:
    static Parameters parameters();
};

} // namespace godzilla
