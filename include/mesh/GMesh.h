#pragma once

#include "base/MooseObject.h"
#include "base/GPrintInterface.h"

/// Base class for meshes
///
class GMesh : public MooseObject,
              public GPrintInterface
{
public:
    GMesh(const InputParameters & parameters);

    /// Create the mesh
    ///
    virtual void create() = 0;

public:
    static InputParameters validParams();
};
