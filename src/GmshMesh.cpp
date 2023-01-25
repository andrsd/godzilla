#include "Godzilla.h"
#include "GmshMesh.h"
#include "App.h"
#include "CallStack.h"
#include "Utils.h"
#include "petscdmplex.h"
#include "petscsys.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace godzilla {

REGISTER_OBJECT(GmshMesh);

Parameters
GmshMesh::parameters()
{
    Parameters params = UnstructuredMesh::parameters();
    params.add_required_param<std::string>("file", "The name of the Gmsh file.");
    return params;
}

GmshMesh::GmshMesh(const Parameters & parameters) :
    UnstructuredMesh(parameters),
    interpolate(PETSC_TRUE)
{
    _F_;

    this->file_name =
        fs::path(this->app->get_input_file_name()).parent_path() / get_param<std::string>("file");

    if (!utils::path_exists(this->file_name))
        log_error(
            "Unable to open '%s' for reading. Make sure it exists and you have read permissions.",
            this->file_name);
}

const std::string &
GmshMesh::get_file_name() const
{
    _F_;
    return this->file_name;
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
