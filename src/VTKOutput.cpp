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
    file_base(getParam<std::string>("file"))
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

const std::string &
VTKOutput::getFileName() const
{
    _F_;
    return this->file_name;
}

void
VTKOutput::setFileName()
{
    _F_;
    PetscErrorCode ierr;
    char fn[MAX_PATH];
    snprintf(fn, MAX_PATH, "%s.vtk", this->file_base.c_str());
    ierr = PetscViewerFileSetName(this->viewer, fn);
    checkPetscError(ierr);
    this->file_name = std::string(fn);
}

void
VTKOutput::setSequenceFileName(unsigned int stepi)
{
    _F_;
    PetscErrorCode ierr;
    char fn[MAX_PATH];
    snprintf(fn, MAX_PATH, "%s.%d.vtk", this->file_base.c_str(), stepi);
    ierr = PetscViewerFileSetName(this->viewer, fn);
    checkPetscError(ierr);
    this->file_name = std::string(fn);
}

void
VTKOutput::output(DM dm, Vec vec) const
{
    _F_;
    PetscErrorCode ierr;
    godzillaPrint(9, "Output to file: ", this->file_name);
    ierr = DMView(dm, this->viewer);
    checkPetscError(ierr);
    ierr = VecView(vec, this->viewer);
    checkPetscError(ierr);
}

} // namespace godzilla
