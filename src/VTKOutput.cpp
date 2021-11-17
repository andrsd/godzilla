#include "Godzilla.h"
#include "VTKOutput.h"
#include "Problem.h"

namespace godzilla {

registerObject(VTKOutput);

static const int MAX_PATH = 1024;

InputParameters
VTKOutput::validParams()
{
    InputParameters params = Output::validParams();
    params.addRequiredParam<std::string>("file", "The name of the output file.");
    return params;
}

VTKOutput::VTKOutput(const InputParameters & params) :
    Output(params),
    file_name(getParam<std::string>("file"))
{
    _F_;
    PetscErrorCode ierr;
    ierr = PetscViewerCreate(comm(), &this->viewer);
    checkPetscError(ierr);
    ierr = PetscViewerSetType(this->viewer, PETSCVIEWERVTK);
    checkPetscError(ierr);
    ierr = PetscViewerFileSetMode(this->viewer, FILE_MODE_WRITE);
    checkPetscError(ierr);
    // TODO: check that DM we are using is DMPLEX. if not, we need to tell
    // users that this does not work.
}

VTKOutput::~VTKOutput()
{
    _F_;
    PetscErrorCode ierr;
    ierr = PetscViewerDestroy(&this->viewer);
    checkPetscError(ierr);
}

void
VTKOutput::setFileName() const
{
    _F_;
    PetscErrorCode ierr;
    char fn[MAX_PATH];
    snprintf(fn, MAX_PATH, "%s.vtk", this->file_name.c_str());
    ierr = PetscViewerFileSetName(this->viewer, fn);
    checkPetscError(ierr);
}

void
VTKOutput::setSequenceFileName(unsigned int stepi) const
{
    _F_;
    PetscErrorCode ierr;
    char fn[MAX_PATH];
    snprintf(fn, MAX_PATH, "%s.%d.vtk", this->file_name.c_str(), stepi);
    ierr = PetscViewerFileSetName(this->viewer, fn);
    checkPetscError(ierr);
}

void
VTKOutput::output(DM dm, Vec vec) const
{
    _F_;
    PetscErrorCode ierr;
    const char * fn;
    ierr = PetscViewerFileGetName(this->viewer, &fn);
    checkPetscError(ierr);
    godzillaPrint(9, "Output to file: ", fn);

    ierr = DMView(dm, this->viewer);
    checkPetscError(ierr);
    ierr = VecView(vec, this->viewer);
    checkPetscError(ierr);
}

} // namespace godzilla
