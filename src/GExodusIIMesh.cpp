#include "Godzilla.h"
#include "GExodusIIMesh.h"
#include "CallStack.h"
#include "Utils.h"
#include "petscdmplex.h"


namespace godzilla {

registerObject(GExodusIIMesh);

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

    if (!utils::pathExists(this->file_name))
        godzillaError("Unable to open '", this->file_name, "' for reading. Make sure it exists and you have read permissions.");
}

const std::string
GExodusIIMesh::getFileName() const
{
    _F_;
    return this->file_name;
}

void
GExodusIIMesh::create()
{
    _F_;
    PetscErrorCode ierr;

    ierr = DMPlexCreateExodusFromFile(comm(),
        this->file_name.c_str(),
        this->interpolate,
        &this->dm);
    checkPetscError(ierr);
    ierr = DMSetUp(this->dm);
    checkPetscError(ierr);
}

}
