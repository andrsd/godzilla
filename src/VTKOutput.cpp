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
    PetscViewerCreate(comm(), &this->viewer);
    PetscViewerSetType(this->viewer, PETSCVIEWERVTK);
    PetscViewerFileSetMode(this->viewer, FILE_MODE_WRITE);
    // TODO: check that DM we are using is DMPLEX. if not, we need to tell
    // users that this does not work.
}

VTKOutput::~VTKOutput()
{
    _F_;
    PetscViewerDestroy(&this->viewer);
}

void
VTKOutput::setFileName() const
{
    _F_;
    char fn[MAX_PATH];
    snprintf(fn, MAX_PATH, "%s.vtk", this->file_name.c_str());
    PetscViewerFileSetName(this->viewer, fn);
}

void
VTKOutput::setSequenceFileName(unsigned int stepi) const
{
    _F_;
    char fn[MAX_PATH];
    snprintf(fn, MAX_PATH, "%s.%d.vtk", this->file_name.c_str(), stepi);
    PetscViewerFileSetName(this->viewer, fn);
}

void
VTKOutput::output(DM dm, Vec vec) const
{
    _F_;
    const char * fn;
    PetscViewerFileGetName(this->viewer, &fn);
    godzillaPrint(9, "Output to file: ", fn);

    DMView(dm, this->viewer);
    VecView(vec, this->viewer);
}

} // namespace godzilla
