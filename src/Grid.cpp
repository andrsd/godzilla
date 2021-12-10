#include "Godzilla.h"
#include "Grid.h"
#include "CallStack.h"

namespace godzilla {

InputParameters
Grid::validParams()
{
    InputParameters params = Object::validParams();
    return params;
}

Grid::Grid(const InputParameters & parameters) :
    Object(parameters),
    PrintInterface(this),
    dm(NULL),
    dim(-1)
{
}

Grid::~Grid()
{
    _F_;
    if (this->dm) {
        PetscErrorCode ierr;
        ierr = DMDestroy(&this->dm);
        checkPetscError(ierr);
    }
}

DM
Grid::getDM() const
{
    _F_;
    return this->dm;
}

PetscInt
Grid::getDimension() const
{
    _F_;
    return this->dim;
}

void
Grid::create()
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
