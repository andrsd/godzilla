#pragma once

#include "mesh/GMesh.h"

/// 1D line
///
class G1DLineMesh : public GMesh
{
public:
    G1DLineMesh(const InputParameters & parameters);

    virtual void create();

protected:
    /// Minimum in the x direction
    const PetscReal & xmin;
    /// Maximum in the x direction
    const PetscReal & xmin;
    /// Number of grid point in the x direction
    const PetscInt & nx;
    /// True for simplices, False for tensor cells
    PetscBool simplex;
    /// create intermediate mesh pieces (edges, faces)
    PetscBool interpolate;

public:
    static InputParameters validParams();
};
