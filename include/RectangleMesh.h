#pragma once

#include "UnstructuredMesh.h"

namespace godzilla {

/// 2D rectangular mesh
///
class RectangleMesh : public UnstructuredMesh {
public:
    explicit RectangleMesh(const Parameters & parameters);

    ///
    NO_DISCARD PetscReal get_x_min() const;
    NO_DISCARD PetscReal get_x_max() const;
    /// Get the number of mesh points in x direction
    NO_DISCARD Int get_nx() const;
    ///
    NO_DISCARD PetscReal get_y_min() const;
    NO_DISCARD PetscReal get_y_max() const;
    /// Get the number of mesh points in y direction
    NO_DISCARD Int get_ny() const;

protected:
    void create_dm() override;

    /// Minimum in the x direction
    const PetscReal & xmin;
    /// Maximum in the x direction
    const PetscReal & xmax;
    /// Minimum in the y direction
    const PetscReal & ymin;
    /// Maximum in the y direction
    const PetscReal & ymax;
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
