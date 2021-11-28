#include "Godzilla.h"
#include "LineMesh.h"
#include "CallStack.h"
#include "petscdm.h"
#include "petscdmplex.h"

namespace godzilla {

registerObject(LineMesh);

InputParameters
LineMesh::validParams()
{
    InputParameters params = UnstructuredMesh::validParams();
    params.addParam<PetscReal>("xmin", 0., "Minimum in the x direction");
    params.addParam<PetscReal>("xmax", 1., "Maximum in the x direction");
    params.addRequiredParam<PetscInt>("nx", "Number of grid points in the x direction");
    return params;
}

LineMesh::LineMesh(const InputParameters & parameters) :
    UnstructuredMesh(parameters),
    xmin(getParam<PetscReal>("xmin")),
    xmax(getParam<PetscReal>("xmax")),
    nx(getParam<PetscInt>("nx")),
    interpolate(PETSC_TRUE)
{
    _F_;
    if (this->xmax <= this->xmin)
        logError("Parameter 'xmax' must be larger than 'xmin'.");
}

PetscReal
LineMesh::getXMin()
{
    _F_;
    return this->xmin;
}

PetscReal
LineMesh::getXMax()
{
    _F_;
    return this->xmax;
}

PetscInt
LineMesh::getNx()
{
    _F_;
    return this->nx;
}

void
LineMesh::create()
{
    _F_;
    PetscErrorCode ierr;

    PetscReal lower[1] = { this->xmin };
    PetscReal upper[1] = { this->xmax };
    PetscInt faces[1] = { this->nx };
    DMBoundaryType periodicity[1] = { DM_BOUNDARY_GHOSTED };

    ierr = DMPlexCreateBoxMesh(comm(),
                               1,
                               PETSC_TRUE,
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

    const char * side_name[] = { "left", "right" };
    for (unsigned int i = 0; i < 2; i++) {
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
