// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Object.h"
#include "godzilla/Types.h"
#include "godzilla/Qtr.h"

namespace godzilla {

class UnstructuredMesh;

/// 1D line
///
class LineMesh : public Object {
public:
    explicit LineMesh(const Parameters & pars);

    /// Get the lower bound in x-direction
    ///
    /// @return Lower bound in x-direction
    Real get_x_min() const;

    /// Get the upper bound in x-direction
    ///
    /// @return Upper bound in x-direction
    Real get_x_max() const;

    /// Get the number of divisions in the x-direction
    ///
    /// @return Number of divisions in the x-direction
    Int get_nx() const;

    Qtr<UnstructuredMesh> create_mesh();

private:
    /// Minimum in the x direction
    Real xmin;
    /// Maximum in the x direction
    Real xmax;
    /// Number of mesh point in the x direction
    Int nx;
    /// create intermediate mesh pieces (edges, faces)
    PetscBool interpolate;

public:
    static Parameters parameters();
};

} // namespace godzilla
