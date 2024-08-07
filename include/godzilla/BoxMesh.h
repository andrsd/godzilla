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
    [[nodiscard]] Real get_x_min() const;

    /// Get upper limit in x-direction
    [[nodiscard]] Real get_x_max() const;

    /// Get the number of mesh points in x direction
    [[nodiscard]] Int get_nx() const;

    /// Get lower limit in y-direction
    [[nodiscard]] Real get_y_min() const;

    /// Get upper limit in y-direction
    [[nodiscard]] Real get_y_max() const;

    /// Get the number of mesh points in y-direction
    [[nodiscard]] Int get_ny() const;

    /// Get lower limit in z-direction
    [[nodiscard]] Real get_z_min() const;

    /// Get upper limit in z-direction
    [[nodiscard]] Real get_z_max() const;

    /// Get the number of mesh points in z direction
    [[nodiscard]] Int get_nz() const;

protected:
    Mesh * create_mesh() override;

private:
    /// Minimum in the x direction
    const Real & xmin;

    /// Maximum in the x direction
    const Real & xmax;

    /// Minimum in the y direction
    const Real & ymin;

    /// Maximum in the y direction
    const Real & ymax;

    /// Minimum in the z direction
    const Real & zmin;

    /// Maximum in the z direction
    const Real & zmax;

    /// Number of mesh point in the x direction
    const Int & nx;

    /// Number of mesh point in the y direction
    const Int & ny;

    /// Number of mesh point in the z direction
    const Int & nz;

    /// True for simplices, False for tensor cells
    PetscBool simplex;

    /// create intermediate mesh pieces (edges, faces)
    PetscBool interpolate;

public:
    /// Method for building Parameters for this class
    static Parameters parameters();
};

} // namespace godzilla
