#pragma once

#include "Object.h"
#include "PrintInterface.h"
#include "petscdm.h"

namespace godzilla {

/// Base class for meshes
///
class Mesh : public Object, public PrintInterface {
public:
    Mesh(const Parameters & parameters);
    virtual ~Mesh();

    DM get_dm() const;

    /// Get the mesh spatial dimension
    ///
    /// @return Mesh spatial dimension
    PetscInt get_dimension() const;

    /// Create the mesh
    virtual void create();

    /// Check if mesh has label with a name
    ///
    /// @param name The name of the label
    /// @return true if label exists, otherwise false
    virtual bool has_label(const std::string & name) const;

    /// Get label associated with a name
    ///
    /// @param name Label name
    /// @return DMLabel associated with the `name`
    virtual DMLabel get_label(const std::string & name) const;

    /// Distribute mesh over processes
    virtual void distribute() = 0;

protected:
    /// Method that builds DM for the mesh
    virtual void create_dm() = 0;

    /// DM object
    DM dm;

    /// Spatial dimension of the mesh
    PetscInt dim;

public:
    static Parameters parameters();
};

} // namespace godzilla
