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
    auto pars = Object::parameters();
    pars.add_required_param<fs::path>("file", "The name of the file.")
        .add_param<bool>("interpolate", true, "Interpolate the mesh")
        .add_param<bool>("distributed", false, "Is the mesh pre-distributed?");
    return pars;
}

FileMesh::FileMesh(const Parameters & pars) :
    Object(pars),
    file_format(UNKNOWN),
    file_name_(pars.get<fs::path>("file")),
    interpolate_(pars.get<bool>("interpolate")),
    distributed_(pars.get<bool>("distributed"))
{
    CALL_STACK_MSG();

    detect_file_format();
    if (this->distributed_) {
        auto local_f_name = get_distributed_file_name(get_comm().rank());
        expect_true(fs::exists(local_f_name),
                    fmt::format("Unable to open '{}' for reading. Make sure it exists and you have "
                                "read permissions.",
                                local_f_name));
    }
    else {
        expect_true(fs::exists(this->file_name_),
                    fmt::format("Unable to open '{}' for reading. Make sure it exists and you have "
                                "read permissions.",
                                this->file_name_));
    }
}

fs::path
FileMesh::get_file_name() const
{
    CALL_STACK_MSG();
    return this->file_name_;
}

fs::path
FileMesh::get_distributed_file_name(int rank) const
{
    CALL_STACK_MSG();
    auto parent = this->file_name_.parent_path();
    auto stem = this->file_name_.stem().string();
    auto ext = this->file_name_.extension().string();
    auto dot_pos = stem.rfind('.');
    if (dot_pos != std::string::npos && dot_pos + 1 < stem.size()) {
        bool all_digits = true;
        for (size_t i = dot_pos + 1; i < stem.size(); ++i) {
            if (!std::isdigit(stem[i])) {
                all_digits = false;
                break;
            }
        }
        if (all_digits)
            stem = stem.substr(0, dot_pos);
    }
    return parent / fmt::format("{}.{}{}", stem, rank, ext);
}

FileMesh::FileFormat
FileMesh::get_file_format() const
{
    CALL_STACK_MSG();
    return this->file_format;
}

bool
FileMesh::is_distributed() const
{
    CALL_STACK_MSG();
    return this->distributed_;
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
    PETSC_CHECK(DMPlexCreateGmshFromFile(get_comm(),
                                         get_file_name().c_str(),
                                         this->interpolate_ ? PETSC_TRUE : PETSC_FALSE,
                                         &dm));
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
    if (this->file_name_.extension() == ".exo" || this->file_name_.extension() == ".e")
        this->file_format = EXODUSII;
    else if (this->file_name_.extension() == ".msh") {
        expect_true(not this->distributed_, "Distributed mesh is not supported for GMSH.");
        this->file_format = GMSH;
    }
}

} // namespace godzilla
