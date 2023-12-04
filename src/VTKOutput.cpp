// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Godzilla.h"
#include "godzilla/CallStack.h"
#include "godzilla/VTKOutput.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/Problem.h"

namespace godzilla {

REGISTER_OBJECT(VTKOutput);

static const int MAX_PATH = 1024;

Parameters
VTKOutput::parameters()
{
    Parameters params = FileOutput::parameters();
    return params;
}

VTKOutput::VTKOutput(const Parameters & params) : FileOutput(params), viewer(nullptr)
{
    _F_;
}

VTKOutput::~VTKOutput()
{
    _F_;
    PETSC_CHECK(PetscViewerDestroy(&this->viewer));
}

std::string
VTKOutput::get_file_ext() const
{
    _F_;
    return { "vtk" };
}

void
VTKOutput::create()
{
    _F_;
    FileOutput::create();

    PETSC_CHECK(PetscViewerCreate(get_comm(), &this->viewer));
    PETSC_CHECK(PetscViewerSetType(this->viewer, PETSCVIEWERVTK));
    PETSC_CHECK(PetscViewerFileSetMode(this->viewer, FILE_MODE_WRITE));
}

void
VTKOutput::check()
{
    _F_;
    FileOutput::check();
    const auto * mesh = dynamic_cast<UnstructuredMesh *>(get_problem()->get_mesh());
    if (mesh == nullptr)
        log_error("VTK output works only with unstructured meshes.");
}

void
VTKOutput::output_step()
{
    _F_;
    auto problem = get_problem();
    set_sequence_file_base(problem->get_step_num());
    PETSC_CHECK(PetscViewerFileSetName(this->viewer, get_file_name().c_str()));

    TIMED_EVENT(9, "VTKOutput", "Output to file: {}", get_file_name());
    auto dm = problem->get_dm();
    PETSC_CHECK(DMView(dm, this->viewer));
    auto vec = problem->get_solution_vector();
    PETSC_CHECK(VecView(vec, this->viewer));
}

} // namespace godzilla
