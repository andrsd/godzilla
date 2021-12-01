#include "Godzilla.h"
#include "ExodusIIMesh.h"
#include "CallStack.h"
#include "Utils.h"
#include "petscdmplex.h"

namespace godzilla {

registerObject(ExodusIIMesh);

InputParameters
ExodusIIMesh::validParams()
{
    InputParameters params = UnstructuredMesh::validParams();
    params.addRequiredParam<std::string>("file", "The name of the ExodusII file.");
    return params;
}

ExodusIIMesh::ExodusIIMesh(const InputParameters & parameters) :
    UnstructuredMesh(parameters),
    file_name(getParam<std::string>("file")),
    interpolate(PETSC_TRUE)
{
    _F_;

    if (!utils::pathExists(this->file_name))
        logError("Unable to open '",
                 this->file_name,
                 "' for reading. Make sure it exists and you have read permissions.");
}

const std::string
ExodusIIMesh::getFileName() const
{
    _F_;
    return this->file_name;
}

void
ExodusIIMesh::create()
{
    _F_;
    PetscErrorCode ierr;

    ierr =
        DMPlexCreateExodusFromFile(comm(), this->file_name.c_str(), this->interpolate, &this->dm);
    checkPetscError(ierr);
    ierr = DMSetUp(this->dm);
    checkPetscError(ierr);
}

} // namespace godzilla
