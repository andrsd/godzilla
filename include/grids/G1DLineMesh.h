#pragma once

#include "grids/GUnstructuredMesh.h"

/// 1D line
///
class G1DLineMesh : public GUnstructuredMesh
{
public:
    G1DLineMesh(const InputParameters & parameters);

    virtual void create();

protected:
    /// Minimum in the x direction
    const PetscReal & xmin;
    /// Maximum in the x direction
    const PetscReal & xmax;
    /// Number of grid point in the x direction
    const PetscInt & nx;
    /// True for simplices, False for tensor cells
    PetscBool simplex;
    /// create intermediate mesh pieces (edges, faces)
    PetscBool interpolate;

public:
    static InputParameters validParams();
};
