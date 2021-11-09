#pragma once

#include "Grid.h"

namespace godzilla {

/// 1D structured grid
///
class StructuredGrid1D : public Grid
{
public:
    StructuredGrid1D(const InputParameters & parameters);

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
