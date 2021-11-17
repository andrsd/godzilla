#include "Godzilla.h"
#include "HDF5Output.h"
#include "Problem.h"
#include "petscviewerhdf5.h"

namespace godzilla {

registerObject(HDF5Output);

static const int MAX_PATH = 1024;

InputParameters
HDF5Output::validParams()
{
    InputParameters params = Output::validParams();
    params.addRequiredParam<std::string>("file", "The name of the output file.");
    return params;
}

HDF5Output::HDF5Output(const InputParameters & params) :
    Output(params),
    file_base(getParam<std::string>("file"))
{
    _F_;
    PetscErrorCode ierr;
    ierr = PetscViewerCreate(comm(), &this->viewer);
    checkPetscError(ierr);
    ierr = PetscViewerSetType(this->viewer, PETSCVIEWERHDF5);
    checkPetscError(ierr);
    ierr = PetscViewerFileSetMode(this->viewer, FILE_MODE_WRITE);
    checkPetscError(ierr);
    // TODO: check that DM we are using is DMPLEX. if not, we need to tell
    // users that this does not work.
}

HDF5Output::~HDF5Output()
{
    _F_;
    PetscErrorCode ierr;
    ierr = PetscViewerDestroy(&this->viewer);
    checkPetscError(ierr);
}

const std::string &
HDF5Output::getFileName() const
{
    _F_;
    return this->file_name;
}

void
HDF5Output::setFileName()
{
    _F_;
    PetscErrorCode ierr;
    char fn[MAX_PATH];
    snprintf(fn, MAX_PATH, "%s.h5", this->file_base.c_str());
    ierr = PetscViewerFileSetName(this->viewer, fn);
    checkPetscError(ierr);
    this->file_name = std::string(fn);
}

void
HDF5Output::setSequenceFileName(unsigned int stepi)
{
    _F_;
    PetscErrorCode ierr;
    char fn[MAX_PATH];
    snprintf(fn, MAX_PATH, "%s.%d.h5", this->file_base.c_str(), stepi);
    ierr = PetscViewerFileSetName(this->viewer, fn);
    checkPetscError(ierr);
    this->file_name = std::string(fn);
}

void
HDF5Output::output(DM dm, Vec vec) const
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
