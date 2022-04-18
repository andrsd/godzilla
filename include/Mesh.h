#pragma once

#include "GodzillaConfig.h"

namespace godzilla {

/// Represents the geometry
///
class Mesh {
public:
    Mesh();
    virtual ~Mesh();
    /// Frees all data associated with the mesh.
    void free();

    /// Returns the total number of elements stored.
    uint get_num_elements() const;

protected:
};

} // namespace godzilla
