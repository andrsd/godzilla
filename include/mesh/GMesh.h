#pragma once

#include "MooseObject.h"
#include "GPrintInterface.h"

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
