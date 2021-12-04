#include "Godzilla.h"
#include "VTKOutput.h"
#include "Problem.h"

namespace godzilla {

registerObject(VTKOutput);

static const int MAX_PATH = 1024;

InputParameters
VTKOutput::validParams()
{
    InputParameters params = FileOutput::validParams();
    return params;
}

VTKOutput::VTKOutput(const InputParameters & params) : FileOutput(params)
{
    _F_;
}

std::string
VTKOutput::getFileExt() const
{
    return std::string("vtk");
}

void
VTKOutput::create()
{
    _F_;
    PetscErrorCode ierr;
    ierr = PetscViewerCreate(comm(), &this->viewer);
    checkPetscError(ierr);
    ierr = PetscViewerSetType(this->viewer, PETSCVIEWERVTK);
    checkPetscError(ierr);
    ierr = PetscViewerFileSetMode(this->viewer, FILE_MODE_WRITE);
    checkPetscError(ierr);
}

void
VTKOutput::check()
{
    _F_;
    // TODO: check that DM we are using is DMPLEX. if not, we need to tell
    // users that this does not work.
}

} // namespace godzilla
