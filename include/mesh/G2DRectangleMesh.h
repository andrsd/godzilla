#pragma once

#include "mesh/GMesh.h"

/// 2D rectangular mesh
///
class G2DRectangleMesh : public GMesh
{
public:
    G2DRectangleMesh(const InputParameters & parameters);

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
