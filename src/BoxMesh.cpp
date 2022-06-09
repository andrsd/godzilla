#include "Godzilla.h"
#include "BoxMesh.h"
#include "CallStack.h"
#include "petscdmplex.h"

namespace godzilla {

registerObject(BoxMesh);

InputParameters
BoxMesh::valid_params()
{
    InputParameters params = UnstructuredMesh::valid_params();
    params.add_param<PetscReal>("xmin", 0., "Minimum in the x direction");
    params.add_param<PetscReal>("xmax", 1., "Maximum in the x direction");
    params.add_param<PetscReal>("ymin", 0., "Minimum in the y direction");
    params.add_param<PetscReal>("ymax", 1., "Maximum in the y direction");
    params.add_param<PetscReal>("zmin", 0., "Minimum in the z direction");
    params.add_param<PetscReal>("zmax", 1., "Maximum in the z direction");
    params.add_required_param<PetscInt>("nx", "Number of mesh points in the x direction");
    params.add_required_param<PetscInt>("ny", "Number of mesh points in the y direction");
    params.add_required_param<PetscInt>("nz", "Number of mesh points in the z direction");
    return params;
}

BoxMesh::BoxMesh(const InputParameters & parameters) :
    UnstructuredMesh(parameters),
    xmin(get_param<PetscReal>("xmin")),
    xmax(get_param<PetscReal>("xmax")),
    ymin(get_param<PetscReal>("ymin")),
    ymax(get_param<PetscReal>("ymax")),
    zmin(get_param<PetscReal>("zmin")),
    zmax(get_param<PetscReal>("zmax")),
    nx(get_param<PetscInt>("nx")),
    ny(get_param<PetscInt>("ny")),
    nz(get_param<PetscInt>("nz")),
    simplex(PETSC_FALSE),
    interpolate(PETSC_TRUE)
{
    _F_;
    if (this->xmax <= this->xmin)
        log_error("Parameter 'xmax' must be larger than 'xmin'.");
    if (this->ymax <= this->ymin)
        log_error("Parameter 'ymax' must be larger than 'ymin'.");
    if (this->zmax <= this->zmin)
        log_error("Parameter 'zmax' must be larger than 'zmin'.");
}

PetscInt
BoxMesh::get_x_min() const
{
    _F_;
    return this->xmin;
}

PetscInt
BoxMesh::get_x_max() const
{
    _F_;
    return this->xmax;
}

PetscInt
BoxMesh::get_nx() const
{
    _F_;
    return this->nx;
}

PetscInt
BoxMesh::get_y_min() const
{
    _F_;
    return this->ymin;
}

PetscInt
BoxMesh::get_y_max() const
{
    _F_;
    return this->ymax;
}

PetscInt
BoxMesh::get_ny() const
{
    _F_;
    return this->ny;
}

PetscInt
BoxMesh::get_z_min() const
{
    _F_;
    return this->zmin;
}

PetscInt
BoxMesh::get_z_max() const
{
    _F_;
    return this->zmax;
}

PetscInt
BoxMesh::get_nz() const
{
    _F_;
    return this->nz;
}

void
BoxMesh::create_dm()
{
    _F_;
    PetscReal lower[3] = { this->xmin, this->ymin, this->zmin };
    PetscReal upper[3] = { this->xmax, this->ymax, this->zmax };
    PetscInt faces[3] = { this->nx, this->ny, this->nz };
    DMBoundaryType periodicity[3] = { DM_BOUNDARY_GHOSTED,
                                      DM_BOUNDARY_GHOSTED,
                                      DM_BOUNDARY_GHOSTED };

    PETSC_CHECK(DMPlexCreateBoxMesh(get_comm(),
                                    3,
                                    this->simplex,
                                    faces,
                                    lower,
                                    upper,
                                    periodicity,
                                    interpolate,
                                    &this->dm));

    // create user-friendly names for sides
    DMLabel face_sets_label = get_label("Face Sets");

    const char * side_name[] = { "back", "front", "bottom", "top", "right", "left" };
    for (unsigned int i = 0; i < 6; i++) {
        IS is;
        PETSC_CHECK(DMLabelGetStratumIS(face_sets_label, i + 1, &is));
        PETSC_CHECK(DMCreateLabel(this->dm, side_name[i]));
        DMLabel label = get_label(side_name[i]);
        if (is)
            PETSC_CHECK(DMLabelSetStratumIS(label, i + 1, is));
        PETSC_CHECK(ISDestroy(&is));
        PETSC_CHECK(DMPlexLabelComplete(this->dm, label));
    }
}

} // namespace godzilla
