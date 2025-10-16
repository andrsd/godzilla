// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/MeshObject.h"
#include "godzilla/Types.h"

namespace godzilla {

/// 3D box mesh
///
class BoxMesh : public MeshObject {
public:
    /// Constructor for building the object via Factory
    explicit BoxMesh(const Parameters & parameters);

    /// Get lower limit in x-direction
    Real get_x_min() const;

    /// Get upper limit in x-direction
    Real get_x_max() const;

    /// Get the number of mesh points in x direction
    Int get_nx() const;

    /// Get lower limit in y-direction
    Real get_y_min() const;

    /// Get upper limit in y-direction
    Real get_y_max() const;

    /// Get the number of mesh points in y-direction
    Int get_ny() const;

    /// Get lower limit in z-direction
    Real get_z_min() const;

    /// Get upper limit in z-direction
    Real get_z_max() const;

    /// Get the number of mesh points in z direction
    Int get_nz() const;

protected:
    Qtr<Mesh> create_mesh() override;

private:
    /// Minimum in the x direction
    Real xmin;

    /// Maximum in the x direction
    Real xmax;

    /// Minimum in the y direction
    Real ymin;

    /// Maximum in the y direction
    Real ymax;

    /// Minimum in the z direction
    Real zmin;

    /// Maximum in the z direction
    Real zmax;

    /// Number of mesh point in the x direction
    Int nx;

    /// Number of mesh point in the y direction
    Int ny;

    /// Number of mesh point in the z direction
    Int nz;

    /// True for simplices, False for tensor cells
    PetscBool simplex;

    /// create intermediate mesh pieces (edges, faces)
    PetscBool interpolate;

public:
    /// Method for building Parameters for this class
    static Parameters parameters();
};

} // namespace godzilla
