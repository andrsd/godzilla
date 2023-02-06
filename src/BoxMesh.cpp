#include "Godzilla.h"
#include "BoxMesh.h"
#include "CallStack.h"
#include "petscdmplex.h"
#include <array>

namespace godzilla {

REGISTER_OBJECT(BoxMesh);

Parameters
BoxMesh::parameters()
{
    Parameters params = UnstructuredMesh::parameters();
    params.add_param<PetscReal>("xmin", 0., "Minimum in the x direction");
    params.add_param<PetscReal>("xmax", 1., "Maximum in the x direction");
    params.add_param<PetscReal>("ymin", 0., "Minimum in the y direction");
    params.add_param<PetscReal>("ymax", 1., "Maximum in the y direction");
    params.add_param<PetscReal>("zmin", 0., "Minimum in the z direction");
    params.add_param<PetscReal>("zmax", 1., "Maximum in the z direction");
    params.add_required_param<Int>("nx", "Number of mesh points in the x direction");
    params.add_required_param<Int>("ny", "Number of mesh points in the y direction");
    params.add_required_param<Int>("nz", "Number of mesh points in the z direction");
    params.add_param<bool>("simplex", false, "Generate simplex elements");
    return params;
}

BoxMesh::BoxMesh(const Parameters & parameters) :
    UnstructuredMesh(parameters),
    xmin(get_param<PetscReal>("xmin")),
    xmax(get_param<PetscReal>("xmax")),
    ymin(get_param<PetscReal>("ymin")),
    ymax(get_param<PetscReal>("ymax")),
    zmin(get_param<PetscReal>("zmin")),
    zmax(get_param<PetscReal>("zmax")),
    nx(get_param<Int>("nx")),
    ny(get_param<Int>("ny")),
    nz(get_param<Int>("nz")),
    simplex(get_param<bool>("simplex") ? PETSC_TRUE : PETSC_FALSE),
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

PetscReal
BoxMesh::get_x_min() const
{
    _F_;
    return this->xmin;
}

PetscReal
BoxMesh::get_x_max() const
{
    _F_;
    return this->xmax;
}

Int
BoxMesh::get_nx() const
{
    _F_;
    return this->nx;
}

PetscReal
BoxMesh::get_y_min() const
{
    _F_;
    return this->ymin;
}

PetscReal
BoxMesh::get_y_max() const
{
    _F_;
    return this->ymax;
}

Int
BoxMesh::get_ny() const
{
    _F_;
    return this->ny;
}

PetscReal
BoxMesh::get_z_min() const
{
    _F_;
    return this->zmin;
}

PetscReal
BoxMesh::get_z_max() const
{
    _F_;
    return this->zmax;
}

Int
BoxMesh::get_nz() const
{
    _F_;
    return this->nz;
}

void
BoxMesh::create_dm()
{
    _F_;
    std::array<PetscReal, 3> lower = { this->xmin, this->ymin, this->zmin };
    std::array<PetscReal, 3> upper = { this->xmax, this->ymax, this->zmax };
    std::array<Int, 3> faces = { this->nx, this->ny, this->nz };
    std::array<DMBoundaryType, 3> periodicity = {
        this->simplex ? DM_BOUNDARY_NONE : DM_BOUNDARY_GHOSTED,
        this->simplex ? DM_BOUNDARY_NONE : DM_BOUNDARY_GHOSTED,
        this->simplex ? DM_BOUNDARY_NONE : DM_BOUNDARY_GHOSTED
    };

    PETSC_CHECK(DMPlexCreateBoxMesh(get_comm(),
                                    3,
                                    this->simplex,
                                    faces.data(),
                                    lower.data(),
                                    upper.data(),
                                    periodicity.data(),
                                    this->interpolate,
                                    &this->dm));

    // create user-friendly names for sides
    std::map<Int, std::string> face_set_names;
    face_set_names[1] = "back";
    face_set_names[2] = "front";
    face_set_names[3] = "bottom";
    face_set_names[4] = "top";
    face_set_names[5] = "right";
    face_set_names[6] = "left";
    create_face_set_labels(face_set_names);
}

} // namespace godzilla
