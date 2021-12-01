#include "Godzilla.h"
#include "BoxMesh.h"
#include "CallStack.h"
#include "petscdmplex.h"

namespace godzilla {

registerObject(BoxMesh);

InputParameters
BoxMesh::validParams()
{
    InputParameters params = UnstructuredMesh::validParams();
    params.addParam<PetscReal>("xmin", 0., "Minimum in the x direction");
    params.addParam<PetscReal>("xmax", 1., "Maximum in the x direction");
    params.addParam<PetscReal>("ymin", 0., "Minimum in the y direction");
    params.addParam<PetscReal>("ymax", 1., "Maximum in the y direction");
    params.addParam<PetscReal>("zmin", 0., "Minimum in the z direction");
    params.addParam<PetscReal>("zmax", 1., "Maximum in the z direction");
    params.addRequiredParam<PetscInt>("nx", "Number of grid points in the x direction");
    params.addRequiredParam<PetscInt>("ny", "Number of grid points in the y direction");
    params.addRequiredParam<PetscInt>("nz", "Number of grid points in the z direction");
    return params;
}

BoxMesh::BoxMesh(const InputParameters & parameters) :
    UnstructuredMesh(parameters),
    xmin(getParam<PetscReal>("xmin")),
    xmax(getParam<PetscReal>("xmax")),
    ymin(getParam<PetscReal>("ymin")),
    ymax(getParam<PetscReal>("ymax")),
    zmin(getParam<PetscReal>("zmin")),
    zmax(getParam<PetscReal>("zmax")),
    nx(getParam<PetscInt>("nx")),
    ny(getParam<PetscInt>("ny")),
    nz(getParam<PetscInt>("nz")),
    simplex(PETSC_FALSE),
    interpolate(PETSC_TRUE)
{
    _F_;
    if (this->xmax <= this->xmin)
        logError("Parameter 'xmax' must be larger than 'xmin'.");
    if (this->ymax <= this->ymin)
        logError("Parameter 'ymax' must be larger than 'ymin'.");
    if (this->zmax <= this->zmin)
        logError("Parameter 'zmax' must be larger than 'zmin'.");
}

PetscInt
BoxMesh::getXMin() const
{
    _F_;
    return this->xmin;
}

PetscInt
BoxMesh::getXMax() const
{
    _F_;
    return this->xmax;
}

PetscInt
BoxMesh::getNx() const
{
    _F_;
    return this->nx;
}

PetscInt
BoxMesh::getYMin() const
{
    _F_;
    return this->ymin;
}

PetscInt
BoxMesh::getYMax() const
{
    _F_;
    return this->ymax;
}

PetscInt
BoxMesh::getNy() const
{
    _F_;
    return this->ny;
}

PetscInt
BoxMesh::getZMin() const
{
    _F_;
    return this->zmin;
}

PetscInt
BoxMesh::getZMax() const
{
    _F_;
    return this->zmax;
}

PetscInt
BoxMesh::getNz() const
{
    _F_;
    return this->nz;
}

void
BoxMesh::create()
{
    _F_;
    PetscErrorCode ierr;

    PetscReal lower[3] = { this->xmin, this->ymin, this->zmin };
    PetscReal upper[3] = { this->xmax, this->ymax, this->zmax };
    PetscInt faces[3] = { this->nx, this->ny, this->nz };
    DMBoundaryType periodicity[3] = { DM_BOUNDARY_GHOSTED,
                                      DM_BOUNDARY_GHOSTED,
                                      DM_BOUNDARY_GHOSTED };

    ierr = DMPlexCreateBoxMesh(comm(),
                               3,
                               this->simplex,
                               faces,
                               lower,
                               upper,
                               periodicity,
                               interpolate,
                               &this->dm);
    checkPetscError(ierr);

    // create user-friendly names for sides
    DMLabel face_sets_label;
    ierr = DMGetLabel(this->dm, "Face Sets", &face_sets_label);

    const char * side_name[] = { "back", "front", "bottom", "top", "right", "left" };
    for (unsigned int i = 0; i < 6; i++) {
        IS is;
        ierr = DMLabelGetStratumIS(face_sets_label, i + 1, &is);
        checkPetscError(ierr);

        ierr = DMCreateLabel(this->dm, side_name[i]);
        checkPetscError(ierr);

        DMLabel label;
        ierr = DMGetLabel(this->dm, side_name[i], &label);
        checkPetscError(ierr);

        ierr = DMLabelSetStratumIS(label, i + 1, is);
        checkPetscError(ierr);

        ierr = ISDestroy(&is);
        checkPetscError(ierr);
    }

    ierr = DMSetUp(this->dm);
    checkPetscError(ierr);
}

} // namespace godzilla
