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

VTKOutput::VTKOutput(const InputParameters & params) : FileOutput(params)
{
    _F_;
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

} // namespace godzilla
