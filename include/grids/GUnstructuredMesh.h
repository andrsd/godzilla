#pragma once

#include "grids/GGrid.h"

/// Base class for unstructured meshes
///
class GUnstructuredMesh : public GGrid
{
public:
    GUnstructuredMesh(const InputParameters & parameters);

public:
    static InputParameters validParams();
};
