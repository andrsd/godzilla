// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Godzilla.h"
#include "godzilla/App.h"
#include "godzilla/FileMesh.h"
#include "godzilla/CallStack.h"
#include "godzilla/Utils.h"
#include "exodusIIcpp/exodusIIcpp.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace godzilla {

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
    _F_;

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
    _F_;
    return this->file_name;
}

void
FileMesh::create()
{
    _F_;
    switch (this->file_format) {
    case EXODUSII:
        create_from_exodus();
        break;
    case GMSH:
        create_from_gmsh();
        break;
    default:
        break;
    }
    set_up();
    lprint_mesh_info();
}

void
FileMesh::check()
{
    _F_;
    if (this->file_format == UNKNOWN)
        log_error("Unknown mesh format");
}

void
FileMesh::create_from_exodus()
{
    _F_;
    DM dm;
    PETSC_CHECK(DMPlexCreateExodusFromFile(get_comm(), get_file_name().c_str(), PETSC_TRUE, &dm));
    set_dm(dm);

    // Ideally we would like to use DMPlexCreateExodus here and get rid of the above
    // DMPlexCreateExodusFromFile, so that we don't open the same file twice. For some reason,
    // that just doesn't work even though this is just exactly what DMPlexCreateExodusFromFile
    // is doing :confused:
    exodusIIcpp::File f(get_file_name(), exodusIIcpp::FileAccess::READ);
    if (f.is_opened()) {
        auto blk_names = f.read_block_names();
        for (auto & it : blk_names) {
            auto name = it.second;
            if (name.empty()) {
                name = std::to_string(it.first);
                create_cell_set(it.first, name);
            }
            else
                create_cell_set(it.first, it.second);
        }

        for (auto it : f.read_side_set_names())
            set_face_set_name(it.first, it.second);

        f.close();
    }
}

void
FileMesh::create_from_gmsh()
{
    _F_;
    PetscOptionsSetValue(nullptr, "-dm_plex_gmsh_use_regions", nullptr);
    DM dm;
    PETSC_CHECK(DMPlexCreateGmshFromFile(get_comm(), get_file_name().c_str(), PETSC_TRUE, &dm));
    set_dm(dm);
}

void
FileMesh::set_file_format(FileFormat fmt)
{
    _F_;
    this->file_format = fmt;
}

void
FileMesh::detect_file_format()
{
    _F_;
    if (utils::has_suffix(this->file_name, ".exo") || utils::has_suffix(this->file_name, ".e"))
        this->file_format = EXODUSII;
    else if (utils::has_suffix(this->file_name, ".gmsh"))
        this->file_format = GMSH;
}

} // namespace godzilla
