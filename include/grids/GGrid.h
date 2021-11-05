#pragma once

#include "base/Object.h"
#include "base/GPrintInterface.h"
#include "petscdm.h"

namespace godzilla {

/// Base class for grids
///
class GGrid : public Object,
              public GPrintInterface
{
public:
    GGrid(const InputParameters & parameters);
    virtual ~GGrid();

    const DM & getDM() const;

    /// Create the grid
    virtual void create() = 0;

protected:
    /// DM object
    DM dm;

public:
    static InputParameters validParams();
};

}
