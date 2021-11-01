#include "grids/G1DLineMesh.h"
#include "base/CallStack.h"
#include "petscdm.h"

registerMooseObject("GodzillaApp", G1DLineMesh);

InputParameters
G1DLineMesh::validParams()
{
    InputParameters params = GUnstructuredMesh::validParams();
    params.addParam<PetscReal>("xmin", 0., "Minimum in the x direction");
    params.addParam<PetscReal>("xmax", 1., "Maximum in the x direction");
    params.addRequiredParam<PetscInt>("nx", "Number of grid points in the x direction");
    return params;
}

G1DLineMesh::G1DLineMesh(const InputParameters & parameters) :
    GUnstructuredMesh(parameters),
    xmin(getParam<PetscReal>("xmin")),
    xmax(getParam<PetscReal>("xmax")),
    nx(getParam<PetscInt>("nx")),
    interpolate(PETSC_TRUE)
{
    _F_;
    if (this->xmax <= this->xmin)
        godzillaError("Parameter 'xmax' must be larger than 'xmin'.");
}

void
G1DLineMesh::create()
{
    PetscErrorCode ierr;

    PetscReal lower[1] = { this->xmin };
    PetscReal upper[1] = { this->xmax };
    PetscInt faces[1] = { this->nx };
    DMBoundaryType periodicity[1] = { DM_BOUNDARY_GHOSTED };

    ierr = DMPlexCreateBoxMesh(comm().get(),
        1,
        PETSC_TRUE,
        faces,
        lower,
        upper,
        periodicity,
        interpolate,
        &this->dm);
    ierr = DMSetUp(this->dm);
}
