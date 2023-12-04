// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "godzilla/Object.h"
#include "godzilla/PrintInterface.h"
#include "godzilla/Label.h"
#include "godzilla/Vector.h"
#include "godzilla/Section.h"
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
    [[nodiscard]] Int get_dimension() const;

    /// Distribute mesh over processes
    virtual void distribute() = 0;

    /// Check if mesh has label with a name
    ///
    /// @param name The name of the label
    /// @return true if label exists, otherwise false
    [[nodiscard]] virtual bool has_label(const std::string & name) const;

    /// Get label associated with a name
    ///
    /// @param name Label name
    /// @return Label associated with the `name`
    [[nodiscard]] virtual Label get_label(const std::string & name) const;

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

    /// Sets up the data structures inside the `DM` object
    void set_up();

protected:
    /// Set the DM
    void set_dm(DM dm);

private:
    /// DM object
    DM dm;

public:
    static Parameters parameters();
};

} // namespace godzilla
