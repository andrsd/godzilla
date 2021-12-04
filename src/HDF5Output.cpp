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
    InputParameters params = FileOutput::validParams();
    return params;
}

HDF5Output::HDF5Output(const InputParameters & params) : FileOutput(params)
{
    _F_;
}

std::string
HDF5Output::getFileExt() const
{
    return std::string("h5");
}

void
HDF5Output::create()
{
    PetscErrorCode ierr;
    ierr = PetscViewerCreate(comm(), &this->viewer);
    checkPetscError(ierr);
    ierr = PetscViewerSetType(this->viewer, PETSCVIEWERHDF5);
    checkPetscError(ierr);
    ierr = PetscViewerFileSetMode(this->viewer, FILE_MODE_WRITE);
    checkPetscError(ierr);
}

void
HDF5Output::check()
{
    _F_;
    // TODO: check that DM we are using is DMPLEX. if not, we need to tell
    // users that this does not work.
}

} // namespace godzilla
