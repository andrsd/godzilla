#pragma once

#include "UnstructuredMesh.h"

namespace godzilla {

/// 2D rectangular mesh
///
class RectangleMesh : public UnstructuredMesh {
public:
    RectangleMesh(const Parameters & parameters);

    ///
    PetscInt get_x_min() const;
    PetscInt get_x_max() const;
    /// Get the number of mesh points in x direction
    PetscInt get_nx() const;
    ///
    PetscInt get_y_min() const;
    PetscInt get_y_max() const;
    /// Get the number of mesh points in y direction
    PetscInt get_ny() const;

protected:
    virtual void create_dm() override;

    /// Minimum in the x direction
    const PetscReal & xmin;
    /// Maximum in the x direction
    const PetscReal & xmax;
    /// Minimum in the y direction
    const PetscReal & ymin;
    /// Maximum in the y direction
    const PetscReal & ymax;
    /// Number of mesh point in the x direction
    const PetscInt & nx;
    /// Number of mesh point in the y direction
    const PetscInt & ny;
    /// True for simplices, False for tensor cells
    PetscBool simplex;
    /// create intermediate mesh pieces (edges, faces)
    PetscBool interpolate;

public:
    static Parameters parameters();
};

} // namespace godzilla
