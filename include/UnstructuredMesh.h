#pragma once

#include "Grid.h"

namespace godzilla {

/// Base class for unstructured meshes
///
class UnstructuredMesh : public Grid {
public:
    UnstructuredMesh(const InputParameters & parameters);

public:
    static InputParameters validParams();
};

} // namespace godzilla
