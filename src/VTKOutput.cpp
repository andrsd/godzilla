#include "Godzilla.h"
#include "CallStack.h"
#include "VTKOutput.h"
#include "UnstructuredMesh.h"
#include "Problem.h"

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
    return std::string("vtk");
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
    const UnstructuredMesh * mesh =
        dynamic_cast<const UnstructuredMesh *>(this->problem->get_mesh());
    if (mesh == nullptr)
        log_error("VTK output works only with unstructured meshes.");
}

void
VTKOutput::output_step()
{
    _F_;
    set_sequence_file_name(this->problem->get_step_num());
    PETSC_CHECK(PetscViewerFileSetName(this->viewer, this->file_name.c_str()));

    lprintf(9, "Output to file: %s", this->file_name);
    DM dm = this->problem->get_dm();
    PETSC_CHECK(DMView(dm, this->viewer));
    Vec vec = this->problem->get_solution_vector();
    PETSC_CHECK(VecView(vec, this->viewer));
}

} // namespace godzilla
