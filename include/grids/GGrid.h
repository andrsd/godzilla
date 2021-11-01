#pragma once

#include "base/MooseObject.h"
#include "base/GPrintInterface.h"
#include "petscdm.h"

/// Base class for grids
///
class GGrid : public MooseObject,
              public GPrintInterface
{
public:
    GGrid(const InputParameters & parameters);
    virtual ~GGrid();

    const DM & getDM() const;

    /// Create the grid
    ///
    /// This should create the `dm` object which should be of the DMPLEX type
    virtual void create() = 0;

protected:
    // DM object
    DM dm;

public:
    static InputParameters validParams();
};
