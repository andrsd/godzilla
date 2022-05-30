#include "Godzilla.h"
#include "CallStack.h"
#include "HDF5Output.h"
#include "Problem.h"
#include "petscviewerhdf5.h"

namespace godzilla {

registerObject(HDF5Output);

static const int MAX_PATH = 1024;

InputParameters
HDF5Output::valid_params()
{
    InputParameters params = FileOutput::valid_params();
    return params;
}

HDF5Output::HDF5Output(const InputParameters & params) : FileOutput(params)
{
    _F_;
}

std::string
HDF5Output::get_file_ext() const
{
    return std::string("h5");
}

void
HDF5Output::create()
{
    _F_;
    FileOutput::create();

    PetscErrorCode ierr;
    ierr = PetscViewerCreate(get_comm(), &this->viewer);
    check_petsc_error(ierr);
    ierr = PetscViewerSetType(this->viewer, PETSCVIEWERHDF5);
    check_petsc_error(ierr);
    ierr = PetscViewerFileSetMode(this->viewer, FILE_MODE_WRITE);
    check_petsc_error(ierr);
}

void
HDF5Output::check()
{
    _F_;
    // TODO: check that DM we are using is DMPLEX. if not, we need to tell
    // users that this does not work.
}

} // namespace godzilla
