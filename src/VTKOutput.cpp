#include "Godzilla.h"
#include "CallStack.h"
#include "VTKOutput.h"
#include "UnstructuredMesh.h"
#include "Problem.h"

namespace godzilla {

registerObject(VTKOutput);

static const int MAX_PATH = 1024;

InputParameters
VTKOutput::valid_params()
{
    InputParameters params = FileOutput::valid_params();
    return params;
}

VTKOutput::VTKOutput(const InputParameters & params) : FileOutput(params), viewer(nullptr)
{
    _F_;
}

VTKOutput::~VTKOutput()
{
    _F_;
    PetscErrorCode ierr;
    ierr = PetscViewerDestroy(&this->viewer);
    check_petsc_error(ierr);
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

    PetscErrorCode ierr;
    ierr = PetscViewerCreate(get_comm(), &this->viewer);
    check_petsc_error(ierr);
    ierr = PetscViewerSetType(this->viewer, PETSCVIEWERVTK);
    check_petsc_error(ierr);
    ierr = PetscViewerFileSetMode(this->viewer, FILE_MODE_WRITE);
    check_petsc_error(ierr);
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
    PetscErrorCode ierr;

    set_sequence_file_name(this->problem->get_step_num());
    ierr = PetscViewerFileSetName(this->viewer, this->file_name.c_str());
    check_petsc_error(ierr);

    lprintf(9, "Output to file: %s", this->file_name);
    DM dm = this->problem->get_dm();
    ierr = DMView(dm, this->viewer);
    check_petsc_error(ierr);
    Vec vec = this->problem->get_solution_vector();
    ierr = VecView(vec, this->viewer);
    check_petsc_error(ierr);
}

} // namespace godzilla
