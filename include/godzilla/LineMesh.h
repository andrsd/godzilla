// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/MeshObject.h"
#include "godzilla/Types.h"

namespace godzilla {

/// 1D line
///
class LineMesh : public MeshObject {
public:
    explicit LineMesh(const Parameters & parameters);

    /// Get the lower bound in x-direction
    ///
    /// @return Lower bound in x-direction
    [[nodiscard]] Real get_x_min() const;

    /// Get the upper bound in x-direction
    ///
    /// @return Upper bound in x-direction
    [[nodiscard]] Real get_x_max() const;

    /// Get the number of divisions in the x-direction
    ///
    /// @return Number of divisions in the x-direction
    [[nodiscard]] Int get_nx() const;

protected:
    Mesh * create_mesh() override;

private:
    /// Minimum in the x direction
    const Real & xmin;
    /// Maximum in the x direction
    const Real & xmax;
    /// Number of mesh point in the x direction
    const Int & nx;
    /// create intermediate mesh pieces (edges, faces)
    PetscBool interpolate;

public:
    static Parameters parameters();
};

} // namespace godzilla
