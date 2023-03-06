#include "Godzilla.h"
#include "GmshMesh.h"
#include "CallStack.h"

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

void
GmshMesh::create_dm()
{
    _F_;
    lprintf(9, "Loading mesh '%s'", this->file_name);
    PetscOptionsSetValue(nullptr, "-dm_plex_gmsh_use_regions", nullptr);
    PETSC_CHECK(DMPlexCreateGmshFromFile(get_comm(),
                                         this->file_name.c_str(),
                                         this->interpolate,
                                         &this->dm));
}

} // namespace godzilla