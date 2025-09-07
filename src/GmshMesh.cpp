// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/GmshMesh.h"
#include "godzilla/CallStack.h"

namespace godzilla {

Parameters
GmshMesh::parameters()
{
    auto params = FileMesh::parameters();
    return params;
}

GmshMesh::GmshMesh(const Parameters & parameters) : FileMesh(parameters)
{
    CALL_STACK_MSG();
    set_file_format(GMSH);
    deprecated("Use `FileMesh` instead.");
}

} // namespace godzilla
