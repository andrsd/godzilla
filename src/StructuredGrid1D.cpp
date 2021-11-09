#include "Godzilla.h"
#include "StructuredGrid1D.h"
#include "CallStack.h"
#include "petscdmda.h"


namespace godzilla {

registerObject(StructuredGrid1D);

InputParameters
StructuredGrid1D::validParams()
{
    InputParameters params = Grid::validParams();
    params.addRequiredParam<PetscInt>("nx", "Number of grid points in the x direction");
    return params;
}

StructuredGrid1D::StructuredGrid1D(const InputParameters & parameters) :
    Grid(parameters),
    nx(getParam<PetscInt>("nx"))
{
    _F_;
}

PetscInt
StructuredGrid1D::getNx() const
{
    _F_;
    return this->nx;
}

void
StructuredGrid1D::create()
{
    _F_;
    PetscErrorCode ierr;

    // TODO: get this from Problem
    PetscInt dofs = 1;
    PetscInt stencil_width = 1;

    ierr = DMDACreate1d(comm(),
        DM_BOUNDARY_NONE,
        this->nx,
        dofs, stencil_width,
        NULL,
        &this->dm);
    checkPetscError(ierr);
    ierr = DMSetUp(this->dm);
    checkPetscError(ierr);
}

}
