#include "Godzilla.h"
#include "FileOutput.h"

namespace godzilla {

static const int MAX_PATH = 1024;

InputParameters
FileOutput::validParams()
{
    InputParameters params = Output::validParams();
    params.addRequiredParam<std::string>("file", "The name of the output file.");
    return params;
}

FileOutput::FileOutput(const InputParameters & params) :
    Output(params),
    file_base(getParam<std::string>("file")),
    viewer(nullptr)
{
    _F_;
}

FileOutput::~FileOutput()
{
    _F_;
    PetscErrorCode ierr;
    ierr = PetscViewerDestroy(&this->viewer);
    checkPetscError(ierr);
}

const std::string &
FileOutput::getFileName() const
{
    _F_;
    return this->file_name;
}

void
FileOutput::setFileName()
{
    _F_;
    PetscErrorCode ierr;
    char fn[MAX_PATH];
    snprintf(fn, MAX_PATH, "%s.%s", this->file_base.c_str(), this->getFileExt().c_str());
    ierr = PetscViewerFileSetName(this->viewer, fn);
    checkPetscError(ierr);
    this->file_name = std::string(fn);
}

void
FileOutput::setSequenceFileName(unsigned int stepi)
{
    _F_;
    PetscErrorCode ierr;
    char fn[MAX_PATH];
    snprintf(fn, MAX_PATH, "%s.%d.%s", this->file_base.c_str(), stepi, this->getFileExt().c_str());
    ierr = PetscViewerFileSetName(this->viewer, fn);
    checkPetscError(ierr);
    this->file_name = std::string(fn);
}

void
FileOutput::outputMesh(DM dm)
{
    _F_;
    PetscErrorCode ierr;
    ierr = DMView(dm, this->viewer);
    checkPetscError(ierr);
}

void
FileOutput::outputSolution(Vec vec)
{
    _F_;
    PetscErrorCode ierr;
    ierr = VecView(vec, this->viewer);
    checkPetscError(ierr);
}

void
FileOutput::outputStep(PetscInt stepi, DM dm, Vec vec)
{
    _F_;
    if (stepi == -1)
        setFileName();
    else
        setSequenceFileName(stepi);
    godzillaPrint(9, "Output to file: ", this->file_name);
    outputMesh(dm);
    outputSolution(vec);
}

} // namespace godzilla
