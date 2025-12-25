// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/App.h"
#include "godzilla/FileMesh.h"
#include "godzilla/Exception.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/CallStack.h"
#include "godzilla/Utils.h"
#include "godzilla/Formatters.h"
#include "petscdmplex.h"

namespace godzilla {

Parameters
FileMesh::parameters()
{
    auto params = Object::parameters();
    params.add_required_param<fs::path>("file", "The name of the file.");
    return params;
}

FileMesh::FileMesh(const Parameters & pars) :
    Object(pars),
    file_format(UNKNOWN),
    file_name(pars.get<fs::path>("file"))

{
    CALL_STACK_MSG();

    expect_true(
        fs::exists(this->file_name),
        "Unable to open '{}' for reading. Make sure it exists and you have read permissions.",
        this->file_name);
    detect_file_format();
}

fs::path
FileMesh::get_file_name() const
{
    CALL_STACK_MSG();
    return this->file_name;
}

FileMesh::FileFormat
FileMesh::get_file_format() const
{
    CALL_STACK_MSG();
    return this->file_format;
}

Qtr<UnstructuredMesh>
FileMesh::create_mesh()
{
    CALL_STACK_MSG();
    if (this->file_format == EXODUSII)
        return create_from_exodus();
    else if (this->file_format == GMSH)
        return create_from_gmsh();
    else {
        expect_true(false, "Unknown mesh format");
        utils::unreachable();
    }
}

Qtr<UnstructuredMesh>
FileMesh::create_from_gmsh()
{
    CALL_STACK_MSG();
    PetscOptionsSetValue(nullptr, "-dm_plex_gmsh_use_regions", nullptr);
    DM dm;
    PETSC_CHECK(DMPlexCreateGmshFromFile(get_comm(), get_file_name().c_str(), PETSC_TRUE, &dm));
    auto m = Qtr<UnstructuredMesh>::alloc(dm);
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
    if (this->file_name.extension() == ".exo" || this->file_name.extension() == ".e")
        this->file_format = EXODUSII;
    else if (this->file_name.extension() == ".msh")
        this->file_format = GMSH;
}

} // namespace godzilla
