#include "Godzilla.h"
#include "ExodusIIOutput.h"
#include "Problem.h"

namespace godzilla {

registerObject(ExodusIIOutput);

static const int MAX_PATH = 1024;

InputParameters
ExodusIIOutput::validParams()
{
    InputParameters params = Output::validParams();
    params.addRequiredParam<std::string>("file", "The name of the output file.");
    return params;
}

ExodusIIOutput::ExodusIIOutput(const InputParameters & params) :
    Output(params),
    file_base(getParam<std::string>("file"))
{
    _F_;
    PetscErrorCode ierr;
    ierr = PetscViewerCreate(comm(), &this->viewer);
    checkPetscError(ierr);
    ierr = PetscViewerSetType(this->viewer, PETSCVIEWEREXODUSII);
    checkPetscError(ierr);
    ierr = PetscViewerFileSetMode(this->viewer, FILE_MODE_WRITE);
    checkPetscError(ierr);
}

ExodusIIOutput::~ExodusIIOutput()
{
    _F_;
    PetscErrorCode ierr;
    ierr = PetscViewerDestroy(&this->viewer);
    checkPetscError(ierr);
}

const std::string &
ExodusIIOutput::getFileName() const
{
    _F_;
    return this->file_name;
}

void
ExodusIIOutput::setFileName()
{
    _F_;
    PetscErrorCode ierr;
    char fn[MAX_PATH];
    snprintf(fn, MAX_PATH, "%s.exo", this->file_base.c_str());
    ierr = PetscViewerFileSetName(this->viewer, fn);
    checkPetscError(ierr);
    this->file_name = std::string(fn);
}

void
ExodusIIOutput::setSequenceFileName(unsigned int stepi)
{
    _F_;
    PetscErrorCode ierr;
    char fn[MAX_PATH];
    snprintf(fn, MAX_PATH, "%s.%d.exo", this->file_base.c_str(), stepi);
    ierr = PetscViewerFileSetName(this->viewer, fn);
    checkPetscError(ierr);
    this->file_name = std::string(fn);
}

void
ExodusIIOutput::check()
{
    _F_;
    const DM & dm = this->problem.getDM();
    PetscInt dim;
    DMGetDimension(dm, &dim);
    if (dim == 1)
        logError("PETSc viewer does not support ExodusII output for 1D problems.");
}

void
ExodusIIOutput::output(DM dm, Vec vec) const
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
