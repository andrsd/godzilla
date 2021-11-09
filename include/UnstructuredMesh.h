#pragma once

#include "GGrid.h"

namespace godzilla {

/// Base class for unstructured meshes
///
class UnstructuredMesh : public GGrid
{
public:
    UnstructuredMesh(const InputParameters & parameters);

public:
    static InputParameters validParams();
};

}
