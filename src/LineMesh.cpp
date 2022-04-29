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
    InputParameters params = Mesh::validParams();
    params.add_param<Real>("xmin", 0., "Minimum in the x direction");
    params.add_param<Real>("xmax", 1., "Maximum in the x direction");
    params.add_required_param<uint>("nx", "Number of grid points in the x direction");
    return params;
}

LineMesh::LineMesh(const InputParameters & parameters) :
    Mesh(parameters),
    xmin(get_param<Real>("xmin")),
    xmax(get_param<Real>("xmax")),
    nx(get_param<uint>("nx"))
{
    _F_;
    if (this->xmax <= this->xmin)
        log_error("Parameter 'xmax' must be larger than 'xmin'.");
}

Real
LineMesh::get_xmin()
{
    _F_;
    return this->xmin;
}

Real
LineMesh::get_xmax()
{
    _F_;
    return this->xmax;
}

uint
LineMesh::get_nx()
{
    _F_;
    return this->nx;
}

void
LineMesh::create_dm()
{
    _F_;
    PetscErrorCode ierr;

    Real lower[1] = { this->xmin };
    Real upper[1] = { this->xmax };
    PetscInt faces[1] = { (PetscInt) this->nx };
    DMBoundaryType periodicity[1] = { DM_BOUNDARY_GHOSTED };

    ierr = DMPlexCreateBoxMesh(comm(),
                               1,
                               PETSC_TRUE,
                               faces,
                               lower,
                               upper,
                               periodicity,
                               PETSC_TRUE,
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

        if (is != nullptr) {
            ierr = DMLabelSetStratumIS(label, i + 1, is);
            checkPetscError(ierr);
        }

        ierr = ISDestroy(&is);
        checkPetscError(ierr);

        ierr = DMPlexLabelComplete(this->dm, label);
        checkPetscError(ierr);
    }
}

} // namespace godzilla
