// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Object.h"
#include "godzilla/Types.h"
#include "godzilla/Qtr.h"

namespace godzilla {

class UnstructuredMesh;

/// 2D rectangular mesh
///
class RectangleMesh : public Object {
public:
    explicit RectangleMesh(const Parameters & pars);

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

    Qtr<UnstructuredMesh> create_mesh();

private:
    /// Minimum in the x direction
    Real xmin;
    /// Maximum in the x direction
    Real xmax;
    /// Minimum in the y direction
    Real ymin;
    /// Maximum in the y direction
    Real ymax;
    /// Number of mesh point in the x direction
    Int nx;
    /// Number of mesh point in the y direction
    Int ny;
    /// True for simplices, False for tensor cells
    bool simplex;
    /// create intermediate mesh pieces (edges, faces)
    bool interpolate;

public:
    static Parameters parameters();
};

} // namespace godzilla
