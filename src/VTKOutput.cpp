// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/CallStack.h"
#include "godzilla/VTKOutput.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/Problem.h"
#include "godzilla/DiscreteProblemInterface.h"
#include "godzilla/MeshObject.h"

namespace godzilla {

Parameters
VTKOutput::parameters()
{
    auto params = FileOutput::parameters();
    params.add_private_param<MeshObject *>("_mesh_obj", nullptr);
    return params;
}

VTKOutput::VTKOutput(const Parameters & params) : FileOutput(params), viewer(nullptr)
{
    CALL_STACK_MSG();
}

VTKOutput::~VTKOutput()
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscViewerDestroy(&this->viewer));
}

std::string
VTKOutput::get_file_ext() const
{
    CALL_STACK_MSG();
    return { "vtk" };
}

void
VTKOutput::create()
{
    CALL_STACK_MSG();
    auto dpi = dynamic_cast<DiscreteProblemInterface *>(get_problem());
    auto mesh =
        dpi ? dpi->get_mesh() : get_param<MeshObject *>("_mesh_obj")->get_mesh<UnstructuredMesh>();
    if (mesh == nullptr)
        log_error("VTK output works only with unstructured meshes.");

    FileOutput::create();

    PETSC_CHECK(PetscViewerCreate(get_comm(), &this->viewer));
    PETSC_CHECK(PetscViewerSetType(this->viewer, PETSCVIEWERVTK));
    PETSC_CHECK(PetscViewerFileSetMode(this->viewer, FILE_MODE_WRITE));
}

void
VTKOutput::output_step()
{
    CALL_STACK_MSG();
    auto problem = get_problem();
    set_sequence_file_base(problem->get_step_num());
    PETSC_CHECK(PetscViewerFileSetName(this->viewer, get_file_name().c_str()));

    auto dm = problem->get_dm();
    PETSC_CHECK(DMView(dm, this->viewer));
    auto vec = problem->get_solution_vector();
    PETSC_CHECK(VecView(vec, this->viewer));
}

} // namespace godzilla
