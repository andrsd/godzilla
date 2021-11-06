#include "Godzilla.h"
#include "G2DStructuredGrid.h"
#include "CallStack.h"
#include "petscdmda.h"


namespace godzilla {

registerObject(G2DStructuredGrid);

InputParameters
G2DStructuredGrid::validParams()
{
    InputParameters params = GGrid::validParams();
    params.addRequiredParam<PetscInt>("nx", "Number of grid points in the x direction");
    params.addRequiredParam<PetscInt>("ny", "Number of grid points in the y direction");
    return params;
}

G2DStructuredGrid::G2DStructuredGrid(const InputParameters & parameters) :
    GGrid(parameters),
    nx(getParam<PetscInt>("nx")),
    ny(getParam<PetscInt>("ny"))
{
    _F_;
}

PetscInt
G2DStructuredGrid::getNx() const
{
    return this->nx;
}

PetscInt
G2DStructuredGrid::getNy() const
{
    return this->ny;
}

void
G2DStructuredGrid::create()
{
    _F_;
    PetscErrorCode ierr;

    // TODO: get this from GProblem
    PetscInt dofs = 1;
    PetscInt stencil_width = 1;

    ierr = DMDACreate2d(comm(),
        DM_BOUNDARY_NONE, DM_BOUNDARY_NONE,
        DMDA_STENCIL_STAR,
        this->nx, this->ny, PETSC_DECIDE, PETSC_DECIDE,
        dofs, stencil_width,
        NULL, NULL,
        &this->dm);
    ierr = DMSetUp(this->dm);
}

}
