#include "Godzilla.h"
#include "LineMesh.h"
#include "CallStack.h"
#include "petscdm.h"
#include "petscdmplex.h"

namespace godzilla {

registerObject(LineMesh);

InputParameters
LineMesh::valid_params()
{
    InputParameters params = UnstructuredMesh::valid_params();
    params.add_param<PetscReal>("xmin", 0., "Minimum in the x direction");
    params.add_param<PetscReal>("xmax", 1., "Maximum in the x direction");
    params.add_required_param<PetscInt>("nx", "Number of mesh points in the x direction");
    return params;
}

LineMesh::LineMesh(const InputParameters & parameters) :
    UnstructuredMesh(parameters),
    xmin(get_param<PetscReal>("xmin")),
    xmax(get_param<PetscReal>("xmax")),
    nx(get_param<PetscInt>("nx")),
    interpolate(PETSC_TRUE)
{
    _F_;
    if (this->xmax <= this->xmin)
        log_error("Parameter 'xmax' must be larger than 'xmin'.");
}

PetscReal
LineMesh::get_x_min()
{
    _F_;
    return this->xmin;
}

PetscReal
LineMesh::get_x_max()
{
    _F_;
    return this->xmax;
}

PetscInt
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

    PetscReal lower[1] = { this->xmin };
    PetscReal upper[1] = { this->xmax };
    PetscInt faces[1] = { this->nx };
    DMBoundaryType periodicity[1] = { DM_BOUNDARY_GHOSTED };

    ierr = DMPlexCreateBoxMesh(get_comm(),
                               1,
                               PETSC_TRUE,
                               faces,
                               lower,
                               upper,
                               periodicity,
                               interpolate,
                               &this->dm);
    check_petsc_error(ierr);

    // create user-friendly names for sides
    DMLabel face_sets_label;
    ierr = DMGetLabel(this->dm, "Face Sets", &face_sets_label);

    const char * side_name[] = { "left", "right" };
    for (unsigned int i = 0; i < 2; i++) {
        IS is;
        ierr = DMLabelGetStratumIS(face_sets_label, i + 1, &is);
        check_petsc_error(ierr);

        ierr = DMCreateLabel(this->dm, side_name[i]);
        check_petsc_error(ierr);

        DMLabel label;
        ierr = DMGetLabel(this->dm, side_name[i], &label);
        check_petsc_error(ierr);

        if (is != nullptr) {
            ierr = DMLabelSetStratumIS(label, i + 1, is);
            check_petsc_error(ierr);
        }

        ierr = ISDestroy(&is);
        check_petsc_error(ierr);

        ierr = DMPlexLabelComplete(this->dm, label);
        check_petsc_error(ierr);
    }
}

} // namespace godzilla
