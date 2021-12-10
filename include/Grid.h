#pragma once

#include "Object.h"
#include "PrintInterface.h"
#include "petscdm.h"

namespace godzilla {

/// Base class for grids
///
class Grid : public Object, public PrintInterface {
public:
    Grid(const InputParameters & parameters);
    virtual ~Grid();

    DM getDM() const;

    /// Get the grid spatial dimension
    ///
    /// @return Grid spatial dimension
    PetscInt getDimension() const;

    /// Create the grid
    virtual void create();

protected:
    /// Method that builds DM for the mesh
    virtual void createDM() = 0;

    /// Distribute mesh over processes
    virtual void distribute() = 0;

    /// DM object
    DM dm;

    /// Spatial dimension of the mesh
    PetscInt dim;

public:
    static InputParameters validParams();
};

} // namespace godzilla
