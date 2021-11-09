#include "Godzilla.h"
#include "G1DStructuredGrid.h"
#include "CallStack.h"
#include "petscdmda.h"


namespace godzilla {

registerObject(G1DStructuredGrid);

InputParameters
G1DStructuredGrid::validParams()
{
    InputParameters params = GGrid::validParams();
    params.addRequiredParam<PetscInt>("nx", "Number of grid points in the x direction");
    return params;
}

G1DStructuredGrid::G1DStructuredGrid(const InputParameters & parameters) :
    GGrid(parameters),
    nx(getParam<PetscInt>("nx"))
{
    _F_;
}

PetscInt
G1DStructuredGrid::getNx() const
{
    _F_;
    return this->nx;
}

void
G1DStructuredGrid::create()
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
