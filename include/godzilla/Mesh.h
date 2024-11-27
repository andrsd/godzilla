// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "godzilla/Label.h"
#include "godzilla/Vector.h"
#include "godzilla/Section.h"
#include "mpicpp-lite/mpicpp-lite.h"
#include "petscdm.h"

namespace mpi = mpicpp_lite;

namespace godzilla {

/// Base class for meshes
///
class Mesh {
public:
    Mesh();
    explicit Mesh(DM dm);
    virtual ~Mesh();

    /// Get the MPI comm this object works on
    mpi::Communicator get_comm() const;

    /// Get the underlying DM object
    ///
    /// @return Underlying PETSc DM
    DM get_dm() const;

    /// Get the mesh spatial dimension
    ///
    /// @return Mesh spatial dimension
    Int get_dimension() const;

    /// Set the topological dimension of the mesh
    ///
    /// @param dim The topological dimension
    void set_dimension(Int dim);

    /// Check if mesh has label with a name
    ///
    /// @param name The name of the label
    /// @return true if label exists, otherwise false
    bool has_label(const std::string & name) const;

    /// Get label associated with a name
    ///
    /// @param name Label name
    /// @return Label associated with the `name`
    Label get_label(const std::string & name) const;

    /// Create label
    ///
    /// @param name Label name
    void create_label(const std::string & name) const;

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

    /// Set the dimension of the embedding space for coordinate values
    ///
    /// @param dim The embedding dimension
    void set_coordinate_dim(Int dim);

    /// Sets a local vector, including ghost points, that holds the coordinates
    ///
    /// @param c coordinate vector
    void set_coordinates_local(const Vector & c);

    /// Sets up the data structures inside the `DM` object
    void set_up();

    /// Add a point to a Label with given value
    ///
    /// @param name The label name
    /// @param point The mesh point
    /// @param value The label value for this point
    void set_label_value(const char * name, Int point, Int value);

    /// Remove a point from a Label with given value
    ///
    /// @param name The label name
    /// @param point The mesh point
    /// @param value The label value for this point
    void clear_label_value(const char * name, Int point, Int value);

    /// View the underlying DM
    ///
    /// @param viewer Viewer
    void view(PetscViewer viewer = PETSC_VIEWER_STDOUT_WORLD);

    /// Gets an array containing the MPI ranks of all the process' neighbors
    ///
    /// @return Array of all neighboring ranks
    std::vector<int> get_neighbors() const;

protected:
    /// Set the DM
    void set_dm(DM dm);

private:
    /// DM object
    DM dm;
};

} // namespace godzilla
