#include "Godzilla.h"
#include "StructuredGrid2D.h"
#include "CallStack.h"
#include "petscdmda.h"

namespace godzilla {

registerObject(StructuredGrid2D);

InputParameters
StructuredGrid2D::validParams()
{
    InputParameters params = Grid::validParams();
    params.addRequiredParam<PetscInt>("nx", "Number of grid points in the x direction");
    params.addRequiredParam<PetscInt>("ny", "Number of grid points in the y direction");
    return params;
}

StructuredGrid2D::StructuredGrid2D(const InputParameters & parameters) :
    Grid(parameters),
    nx(getParam<PetscInt>("nx")),
    ny(getParam<PetscInt>("ny"))
{
    _F_;
}

PetscInt
StructuredGrid2D::getNx() const
{
    _F_;
    return this->nx;
}

PetscInt
StructuredGrid2D::getNy() const
{
    _F_;
    return this->ny;
}

void
StructuredGrid2D::createDM()
{
    _F_;
    PetscErrorCode ierr;

    // TODO: get this from Problem
    PetscInt dofs = 1;
    PetscInt stencil_width = 1;

    ierr = DMDACreate2d(comm(),
                        DM_BOUNDARY_NONE,
                        DM_BOUNDARY_NONE,
                        DMDA_STENCIL_STAR,
                        this->nx,
                        this->ny,
                        PETSC_DECIDE,
                        PETSC_DECIDE,
                        dofs,
                        stencil_width,
                        NULL,
                        NULL,
                        &this->dm);
    checkPetscError(ierr);
}

void
StructuredGrid2D::distribute()
{
    // FIXME: implement this
}

} // namespace godzilla
