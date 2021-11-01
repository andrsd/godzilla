#pragma once

#include "grids/GGrid.h"

/// 2D structured grid
///
class G2DStructuredGrid : public GGrid
{
public:
    G2DStructuredGrid(const InputParameters & parameters);

    virtual void create();

    /// Get the number of grid points in x direction
    PetscInt getNx() const;
    /// Get the number of grid points in y direction
    PetscInt getNy() const;

protected:
    /// Number of grid point in the x direction
    const PetscInt & nx;
    /// Number of grid point in the y direction
    const PetscInt & ny;

public:
    static InputParameters validParams();
};
