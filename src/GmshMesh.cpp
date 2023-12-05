// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Godzilla.h"
#include "godzilla/GmshMesh.h"
#include "godzilla/CallStack.h"

namespace godzilla {

REGISTER_OBJECT(GmshMesh);

Parameters
GmshMesh::parameters()
{
    Parameters params = FileMesh::parameters();
    return params;
}

GmshMesh::GmshMesh(const Parameters & parameters) : FileMesh(parameters)
{
    _F_;
    set_file_format(GMSH);
}

} // namespace godzilla
