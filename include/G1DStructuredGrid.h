#pragma once

#include "GGrid.h"

namespace godzilla {

/// 1D structured grid
///
class G1DStructuredGrid : public GGrid
{
public:
    G1DStructuredGrid(const InputParameters & parameters);

    /// Get the number of grid points in x direction
    PetscInt getNx() const;

    virtual void create();

protected:
    /// Number of grid point in the x direction
    const PetscInt & nx;

public:
    static InputParameters validParams();
};

}
