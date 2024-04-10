// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Godzilla.h"
#include "godzilla/App.h"
#include "godzilla/FileMesh.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/CallStack.h"
#include "godzilla/Utils.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace godzilla {

Parameters
FileMesh::parameters()
{
    Parameters params = MeshObject::parameters();
    params.add_required_param<std::string>("file", "The name of the file.");
    return params;
}

FileMesh::FileMesh(const Parameters & parameters) :
    MeshObject(parameters),
    file_format(UNKNOWN)
{
    CALL_STACK_MSG();

    std::filesystem::path file(get_param<std::string>("file"));
    if (file.is_absolute())
        this->file_name = file;
    else
        this->file_name = fs::path(get_app()->get_input_file_name()).parent_path() / file;

    if (utils::path_exists(this->file_name))
        detect_file_format();
    else
        log_error(
            "Unable to open '{}' for reading. Make sure it exists and you have read permissions.",
            this->file_name);
}

const std::string &
FileMesh::get_file_name() const
{
    CALL_STACK_MSG();
    return this->file_name;
}

godzilla::Mesh *
FileMesh::create_mesh()
{
    CALL_STACK_MSG();
    if (this->file_format == EXODUSII)
        return create_from_exodus();
    else if (this->file_format == GMSH)
        return create_from_gmsh();
    else {
        log_error("Unknown mesh format");
        return nullptr;
    }
}

godzilla::UnstructuredMesh *
FileMesh::create_from_gmsh()
{
    CALL_STACK_MSG();
    PetscOptionsSetValue(nullptr, "-dm_plex_gmsh_use_regions", nullptr);
    DM dm;
    PETSC_CHECK(DMPlexCreateGmshFromFile(get_comm(), get_file_name().c_str(), PETSC_TRUE, &dm));
    auto m = new UnstructuredMesh(dm);
    return m;
}

void
FileMesh::set_file_format(FileFormat fmt)
{
    CALL_STACK_MSG();
    this->file_format = fmt;
}

void
FileMesh::detect_file_format()
{
    CALL_STACK_MSG();
    if (utils::has_suffix(this->file_name, ".exo") || utils::has_suffix(this->file_name, ".e"))
        this->file_format = EXODUSII;
    else if (utils::has_suffix(this->file_name, ".msh"))
        this->file_format = GMSH;
}

} // namespace godzilla
