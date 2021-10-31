#pragma once

#include "base/MooseObject.h"
#include "base/GPrintInterface.h"
#include "petscdm.h"

/// Base class for meshes
///
class GMesh : public MooseObject,
              public GPrintInterface
{
public:
    GMesh(const InputParameters & parameters);
    virtual ~GMesh();

    const DM & getDM() const;

    /// Create the mesh
    ///
    /// This should create the `dm` object which should be of the DMPLEX type
    virtual void create() = 0;

protected:
    // DMPLEX object
    DM dm;

public:
    static InputParameters validParams();
};
