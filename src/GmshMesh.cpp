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
}

DM
GmshMesh::create_dm()
{
    _F_;
    TIMED_EVENT(9, "MeshLoad", "Loading mesh '{}'", get_file_name());
    PetscOptionsSetValue(nullptr, "-dm_plex_gmsh_use_regions", nullptr);
    DM dm;
    PETSC_CHECK(DMPlexCreateGmshFromFile(get_comm(), get_file_name().c_str(), PETSC_TRUE, &dm));
    return dm;
}

} // namespace godzilla
