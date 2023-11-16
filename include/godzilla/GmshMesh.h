#pragma once

#include "godzilla/FileMesh.h"

namespace godzilla {

/// Mesh loaded from a Gmsh file
///
class GmshMesh : public FileMesh {
public:
    explicit GmshMesh(const Parameters & parameters);

protected:
    void create_dm() override;

public:
    static Parameters parameters();
};

} // namespace godzilla
