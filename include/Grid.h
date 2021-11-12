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

    const DM & getDM() const;

    /// Create the grid
    virtual void create() = 0;

protected:
    /// DM object
    DM dm;

public:
    static InputParameters validParams();
};

} // namespace godzilla
