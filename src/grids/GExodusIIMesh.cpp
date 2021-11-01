#include "grids/GExodusIIMesh.h"
#include "base/CallStack.h"
#include "base/MooseApp.h"
#include "petscdmplex.h"

registerMooseObject("GodzillaApp", GExodusIIMesh);

InputParameters
GExodusIIMesh::validParams()
{
    InputParameters params = GUnstructuredMesh::validParams();
    params.addRequiredParam<std::string>("file", "The name of the ExodusII file.");
    return params;
}

GExodusIIMesh::GExodusIIMesh(const InputParameters & parameters) :
    GUnstructuredMesh(parameters),
    file_name(getParam<std::string>("file")),
    interpolate(PETSC_TRUE)
{
    _F_;

    if (!MooseUtils::pathExists(this->file_name))
        godzillaError("Unable to open '", this->file_name, "' for reading. Make sure it exists and you have read permissions.");
}

void
GExodusIIMesh::create()
{
    _F_;
    PetscErrorCode ierr;

    ierr = DMPlexCreateExodusFromFile(this->comm().get(),
        this->file_name.c_str(),
        this->interpolate,
        &this->dm);
    ierr = DMSetUp(this->dm);
}
