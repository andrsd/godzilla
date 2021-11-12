#pragma once

#include "UnstructuredMesh.h"

namespace godzilla {

/// 2D rectangular mesh
///
class RectangleMesh : public UnstructuredMesh {
public:
    RectangleMesh(const InputParameters & parameters);

    ///
    PetscInt getXMin() const;
    PetscInt getXMax() const;
    /// Get the number of grid points in x direction
    PetscInt getNx() const;
    ///
    PetscInt getYMin() const;
    PetscInt getYMax() const;
    /// Get the number of grid points in y direction
    PetscInt getNy() const;

    virtual void create();

protected:
    /// Minimum in the x direction
    const PetscReal & xmin;
    /// Maximum in the x direction
    const PetscReal & xmax;
    /// Minimum in the y direction
    const PetscReal & ymin;
    /// Maximum in the y direction
    const PetscReal & ymax;
    /// Number of grid point in the x direction
    const PetscInt & nx;
    /// Number of grid point in the y direction
    const PetscInt & ny;
    /// True for simplices, False for tensor cells
    PetscBool simplex;
    /// create intermediate mesh pieces (edges, faces)
    PetscBool interpolate;

public:
    static InputParameters validParams();
};

} // namespace godzilla
