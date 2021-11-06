#include "Godzilla.h"
#include "outputs/GVTKOutput.h"
#include "problems/GProblem.h"

namespace godzilla {

registerObject(GVTKOutput);

static const int MAX_PATH = 1024;

InputParameters
GVTKOutput::validParams() {
    InputParameters params = GOutput::validParams();
    params.addRequiredParam<std::string>("file", "The name of the output file.");
    return params;
}

GVTKOutput::GVTKOutput(const InputParameters & params) :
    GOutput(params),
    file_name(getParam<std::string>("file"))
{
    _F_;
    PetscViewerCreate(comm(), &this->viewer);
    PetscViewerSetType(this->viewer, PETSCVIEWERVTK);
    PetscViewerFileSetMode(this->viewer, FILE_MODE_WRITE);
    // TODO: check that DM we are using is DMPLEX. if not, we need to tell
    // users that this does not work.

}

GVTKOutput::~GVTKOutput()
{
    PetscViewerDestroy(&viewer);
}

void
GVTKOutput::output() const
{
    _F_;
    char fn[MAX_PATH];
    snprintf(fn, MAX_PATH, "%s", this->file_name.c_str());
    PetscViewerFileSetName(this->viewer, fn);

    godzillaPrint(9, "Output to file: ", fn);

    const DM & dm = this->problem.getDM();
    DMView(dm, this->viewer);
    const Vec & sln = this->problem.getSolutionVector();
    VecView(sln, this->viewer);

}

} // godzilla
