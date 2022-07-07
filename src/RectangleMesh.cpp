#include "Godzilla.h"
#include "RectangleMesh.h"
#include "CallStack.h"
#include "petscdmplex.h"

namespace godzilla {

REGISTER_OBJECT(RectangleMesh);

Parameters
RectangleMesh::parameters()
{
    Parameters params = UnstructuredMesh::parameters();
    params.add_param<PetscReal>("xmin", 0., "Minimum in the x direction");
    params.add_param<PetscReal>("xmax", 1., "Maximum in the x direction");
    params.add_param<PetscReal>("ymin", 0., "Minimum in the y direction");
    params.add_param<PetscReal>("ymax", 1., "Maximum in the y direction");
    params.add_required_param<PetscInt>("nx", "Number of mesh points in the x direction");
    params.add_required_param<PetscInt>("ny", "Number of mesh points in the y direction");
    return params;
}

RectangleMesh::RectangleMesh(const Parameters & parameters) :
    UnstructuredMesh(parameters),
    xmin(get_param<PetscReal>("xmin")),
    xmax(get_param<PetscReal>("xmax")),
    ymin(get_param<PetscReal>("ymin")),
    ymax(get_param<PetscReal>("ymax")),
    nx(get_param<PetscInt>("nx")),
    ny(get_param<PetscInt>("ny")),
    simplex(PETSC_FALSE),
    interpolate(PETSC_TRUE)
{
    _F_;
    if (this->xmax <= this->xmin)
        log_error("Parameter 'xmax' must be larger than 'xmin'.");
    if (this->ymax <= this->ymin)
        log_error("Parameter 'ymax' must be larger than 'ymin'.");
}

PetscInt
RectangleMesh::get_x_min() const
{
    _F_;
    return this->xmin;
}

PetscInt
RectangleMesh::get_x_max() const
{
    _F_;
    return this->xmax;
}

PetscInt
RectangleMesh::get_nx() const
{
    _F_;
    return this->nx;
}

PetscInt
RectangleMesh::get_y_min() const
{
    _F_;
    return this->ymin;
}

PetscInt
RectangleMesh::get_y_max() const
{
    _F_;
    return this->ymax;
}

PetscInt
RectangleMesh::get_ny() const
{
    _F_;
    return this->ny;
}

void
RectangleMesh::create_dm()
{
    _F_;
    PetscErrorCode ierr;

    PetscReal lower[2] = { this->xmin, this->ymin };
    PetscReal upper[2] = { this->xmax, this->ymax };
    PetscInt faces[2] = { this->nx, this->ny };
    DMBoundaryType periodicity[2] = { DM_BOUNDARY_GHOSTED, DM_BOUNDARY_GHOSTED };

    PETSC_CHECK(DMPlexCreateBoxMesh(get_comm(),
                                    2,
                                    this->simplex,
                                    faces,
                                    lower,
                                    upper,
                                    periodicity,
                                    interpolate,
                                    &this->dm));

    // create user-friendly names for sides
    std::map<int, std::string> face_set_names;
    face_set_names[1] = "bottom";
    face_set_names[2] = "right";
    face_set_names[3] = "top";
    face_set_names[4] = "left";
    create_face_set_labels(face_set_names);
}

} // namespace godzilla
