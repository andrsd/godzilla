#pragma once

#include "Grid.h"

namespace godzilla {

/// 2D structured grid
///
class StructuredGrid2D : public Grid {
public:
    StructuredGrid2D(const InputParameters & parameters);

    /// Get the number of grid points in x direction
    PetscInt getNx() const;
    /// Get the number of grid points in y direction
    PetscInt getNy() const;

protected:
    virtual void createDM() override;
    virtual void distribute() override;

    /// Number of grid point in the x direction
    const PetscInt & nx;
    /// Number of grid point in the y direction
    const PetscInt & ny;

public:
    static InputParameters validParams();
};

} // namespace godzilla
