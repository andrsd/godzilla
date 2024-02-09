// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Godzilla.h"
#include "godzilla/App.h"
#include "godzilla/FileMesh.h"
#include "godzilla/CallStack.h"
#include "godzilla/Utils.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace godzilla {

REGISTER_OBJECT(FileMesh);

Parameters
FileMesh::parameters()
{
    Parameters params = UnstructuredMesh::parameters();
    params.add_required_param<std::string>("file", "The name of the file.");
    return params;
}

FileMesh::FileMesh(const Parameters & parameters) :
    UnstructuredMesh(parameters),
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

void
FileMesh::create()
{
    CALL_STACK_MSG();
    switch (this->file_format) {
    case EXODUSII:
        create_from_exodus();
        break;
    case GMSH:
        create_from_gmsh();
        break;
    default:
        log_error("Unknown mesh format");
        return;
    }
    set_up();
    lprint_mesh_info();
}

void
FileMesh::create_from_gmsh()
{
    CALL_STACK_MSG();
    PetscOptionsSetValue(nullptr, "-dm_plex_gmsh_use_regions", nullptr);
    DM dm;
    PETSC_CHECK(DMPlexCreateGmshFromFile(get_comm(), get_file_name().c_str(), PETSC_TRUE, &dm));
    set_dm(dm);
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
