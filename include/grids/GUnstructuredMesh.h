#pragma once

#include "grids/GGrid.h"

namespace godzilla {

/// Base class for unstructured meshes
///
class GUnstructuredMesh : public GGrid
{
public:
    GUnstructuredMesh(const InputParameters & parameters);

public:
    static InputParameters validParams();
};

}
