#pragma once

#include "GodzillaConfig.h"
#include "Types.h"
#include "Object.h"
#include "PrintInterface.h"
#include "Label.h"
#include "Vector.h"
#include "Section.h"
#include "petscdm.h"

namespace godzilla {

/// Base class for meshes
///
class Mesh : public Object, public PrintInterface {
public:
    explicit Mesh(const Parameters & parameters);
    virtual ~Mesh();

    /// Get the underlying DM object
    ///
    /// @return Underlying PETSc DM
    DM get_dm() const;

    /// Get the mesh spatial dimension
    ///
    /// @return Mesh spatial dimension
    NO_DISCARD Int get_dimension() const;

    /// Distribute mesh over processes
    virtual void distribute() = 0;

    /// Check if mesh has label with a name
    ///
    /// @param name The name of the label
    /// @return true if label exists, otherwise false
    NO_DISCARD virtual bool has_label(const std::string & name) const;

    /// Get label associated with a name
    ///
    /// @param name Label name
    /// @return Label associated with the `name`
    NO_DISCARD virtual Label get_label(const std::string & name) const;

    /// Create label
    ///
    /// @param name Label name
    /// @return Created label
    Label create_label(const std::string & name) const;

    /// Remove label with given name
    ///
    /// @param name The label name
    void remove_label(const std::string & name);

    /// Gets the DM that prescribes coordinate layout and scatters between global and local
    /// coordinates
    ///
    /// @return coordinate `DM`
    DM get_coordinate_dm() const;

    /// Get a global vector with the coordinates associated with this mesh
    ///
    /// @return Global coordinate vector
    Vector get_coordinates() const;

    /// Get a local vector with the coordinates associated with this mesh
    ///
    /// @return Coordinate vector
    Vector get_coordinates_local() const;

    /// Retrieve the layout of coordinate values over the mesh.
    ///
    /// @return Local section from the coordinate `DM`
    Section get_coordinate_section() const;

protected:
    /// DM object
    DM dm;
    /// Spatial dimension of the mesh
    Int dim;

public:
    static Parameters parameters();
};

} // namespace godzilla
