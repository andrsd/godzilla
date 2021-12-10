#pragma once

#include "Grid.h"

namespace godzilla {

/// 1D structured grid
///
class StructuredGrid1D : public Grid {
public:
    StructuredGrid1D(const InputParameters & parameters);

    /// Get the number of grid points in x direction
    PetscInt getNx() const;

protected:
    virtual void createDM() override;
    virtual void distribute() override;

    /// Number of grid point in the x direction
    const PetscInt & nx;

public:
    static InputParameters validParams();
};

} // namespace godzilla
