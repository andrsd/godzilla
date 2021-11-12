#include "Godzilla.h"
#include "RectangleMesh.h"
#include "CallStack.h"
#include "petscdmplex.h"

namespace godzilla {

registerObject(RectangleMesh);

InputParameters
RectangleMesh::validParams()
{
    InputParameters params = UnstructuredMesh::validParams();
    params.addParam<PetscReal>("xmin", 0., "Minimum in the x direction");
    params.addParam<PetscReal>("xmax", 1., "Maximum in the x direction");
    params.addParam<PetscReal>("ymin", 0., "Minimum in the y direction");
    params.addParam<PetscReal>("ymax", 1., "Maximum in the y direction");
    params.addRequiredParam<PetscInt>("nx", "Number of grid points in the x direction");
    params.addRequiredParam<PetscInt>("ny", "Number of grid points in the y direction");
    return params;
}

RectangleMesh::RectangleMesh(const InputParameters & parameters) :
    UnstructuredMesh(parameters),
    xmin(getParam<PetscReal>("xmin")),
    xmax(getParam<PetscReal>("xmax")),
    ymin(getParam<PetscReal>("ymin")),
    ymax(getParam<PetscReal>("ymax")),
    nx(getParam<PetscInt>("nx")),
    ny(getParam<PetscInt>("ny")),
    simplex(PETSC_FALSE),
    interpolate(PETSC_TRUE)
{
    _F_;
    if (this->xmax <= this->xmin)
        godzillaError("Parameter 'xmax' must be larger than 'xmin'.");
    if (this->ymax <= this->ymin)
        godzillaError("Parameter 'ymax' must be larger than 'ymin'.");
}

PetscInt
RectangleMesh::getXMin() const
{
    _F_;
    return this->xmin;
}

PetscInt
RectangleMesh::getXMax() const
{
    _F_;
    return this->xmax;
}

PetscInt
RectangleMesh::getNx() const
{
    _F_;
    return this->nx;
}

PetscInt
RectangleMesh::getYMin() const
{
    _F_;
    return this->ymin;
}

PetscInt
RectangleMesh::getYMax() const
{
    _F_;
    return this->ymax;
}

PetscInt
RectangleMesh::getNy() const
{
    _F_;
    return this->ny;
}

void
RectangleMesh::create()
{
    _F_;
    PetscErrorCode ierr;

    PetscReal lower[2] = { this->xmin, this->ymin };
    PetscReal upper[2] = { this->xmax, this->ymax };
    PetscInt faces[2] = { this->nx, this->ny };
    DMBoundaryType periodicity[2] = { DM_BOUNDARY_GHOSTED, DM_BOUNDARY_GHOSTED };

    ierr = DMPlexCreateBoxMesh(comm(),
                               2,
                               this->simplex,
                               faces,
                               lower,
                               upper,
                               periodicity,
                               interpolate,
                               &this->dm);
    checkPetscError(ierr);
    ierr = DMSetUp(this->dm);
    checkPetscError(ierr);
}

} // namespace godzilla
