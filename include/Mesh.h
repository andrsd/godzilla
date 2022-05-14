#pragma once

#include "Object.h"
#include "PrintInterface.h"
#include "petscdm.h"

namespace godzilla {

/// Base class for meshes
///
class Mesh : public Object, public PrintInterface {
public:
    Mesh(const InputParameters & parameters);
    virtual ~Mesh();

    DM get_dm() const;

    /// Get the mesh spatial dimension
    ///
    /// @return Mesh spatial dimension
    PetscInt get_dimension() const;

    /// Create the mesh
    virtual void create();

protected:
    /// Method that builds DM for the mesh
    virtual void create_dm() = 0;

    /// Distribute mesh over processes
    virtual void distribute() = 0;

    /// DM object
    DM dm;

    /// Spatial dimension of the mesh
    PetscInt dim;

public:
    static InputParameters valid_params();
};

} // namespace godzilla
