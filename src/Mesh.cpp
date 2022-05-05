#include "Godzilla.h"
#include "Mesh.h"
#include "CallStack.h"

namespace godzilla {

InputParameters
Mesh::validParams()
{
    InputParameters params = Object::validParams();
    return params;
}

Mesh::Mesh(const InputParameters & parameters) :
    Object(parameters),
    PrintInterface(this),
    dm(NULL),
    dim(-1)
{
}

Mesh::~Mesh()
{
    _F_;
    if (this->dm) {
        PetscErrorCode ierr;
        ierr = DMDestroy(&this->dm);
        checkPetscError(ierr);
    }
}

DM
Mesh::getDM() const
{
    _F_;
    return this->dm;
}

PetscInt
Mesh::getDimension() const
{
    _F_;
    return this->dim;
}

void
Mesh::create()
{
    _F_;
    PetscErrorCode ierr;

    createDM();
    ierr = DMSetUp(this->dm);
    checkPetscError(ierr);
    ierr = DMGetDimension(this->dm, &this->dim);
    checkPetscError(ierr);
    distribute();
}

} // namespace godzilla
