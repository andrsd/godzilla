// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/MeshObject.h"
#include "godzilla/Types.h"

namespace godzilla {

/// 2D rectangular mesh
///
class RectangleMesh : public MeshObject {
public:
    explicit RectangleMesh(const Parameters & parameters);

    ///
    Real get_x_min() const;
    Real get_x_max() const;
    /// Get the number of mesh points in x direction
    Int get_nx() const;
    ///
    Real get_y_min() const;
    Real get_y_max() const;
    /// Get the number of mesh points in y direction
    Int get_ny() const;

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
    /// Number of mesh point in the x direction
    const Int & nx;
    /// Number of mesh point in the y direction
    const Int & ny;
    /// True for simplices, False for tensor cells
    PetscBool simplex;
    /// create intermediate mesh pieces (edges, faces)
    PetscBool interpolate;

public:
    static Parameters parameters();
};

} // namespace godzilla
