#pragma once

#include "base/MooseObject.h"
#include "base/GPrintInterface.h"

/// Mesh
///
class GMesh : public MooseObject,
              public GPrintInterface
{
public:
    GMesh(const InputParameters & parameters);

public:
    static InputParameters validParams();
};
