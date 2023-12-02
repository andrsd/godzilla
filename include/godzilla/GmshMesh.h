// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/FileMesh.h"

namespace godzilla {

/// Mesh loaded from a Gmsh file
///
class GmshMesh : public FileMesh {
public:
    explicit GmshMesh(const Parameters & parameters);

public:
    static Parameters parameters();
};

} // namespace godzilla
