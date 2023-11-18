#include "godzilla/Godzilla.h"
#include "godzilla/BoxMesh.h"
#include "godzilla/CallStack.h"
#include "petscdmplex.h"
#include <array>

namespace godzilla {

REGISTER_OBJECT(BoxMesh);

Parameters
BoxMesh::parameters()
{
    Parameters params = UnstructuredMesh::parameters();
    params.add_param<Real>("xmin", 0., "Minimum in the x direction");
    params.add_param<Real>("xmax", 1., "Maximum in the x direction");
    params.add_param<Real>("ymin", 0., "Minimum in the y direction");
    params.add_param<Real>("ymax", 1., "Maximum in the y direction");
    params.add_param<Real>("zmin", 0., "Minimum in the z direction");
    params.add_param<Real>("zmax", 1., "Maximum in the z direction");
    params.add_required_param<Int>("nx", "Number of mesh points in the x direction");
    params.add_required_param<Int>("ny", "Number of mesh points in the y direction");
    params.add_required_param<Int>("nz", "Number of mesh points in the z direction");
    params.add_param<bool>("simplex", false, "Generate simplex elements");
    return params;
}

BoxMesh::BoxMesh(const Parameters & parameters) :
    UnstructuredMesh(parameters),
    xmin(get_param<Real>("xmin")),
    xmax(get_param<Real>("xmax")),
    ymin(get_param<Real>("ymin")),
    ymax(get_param<Real>("ymax")),
    zmin(get_param<Real>("zmin")),
    zmax(get_param<Real>("zmax")),
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

Real
BoxMesh::get_x_min() const
{
    _F_;
    return this->xmin;
}

Real
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

Real
BoxMesh::get_y_min() const
{
    _F_;
    return this->ymin;
}

Real
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

Real
BoxMesh::get_z_min() const
{
    _F_;
    return this->zmin;
}

Real
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
    std::array<Real, 3> lower = { this->xmin, this->ymin, this->zmin };
    std::array<Real, 3> upper = { this->xmax, this->ymax, this->zmax };
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

    remove_label("marker");
    // create user-friendly names for sides
    std::map<Int, std::string> face_set_names;
    face_set_names[1] = "back";
    face_set_names[2] = "front";
    face_set_names[3] = "bottom";
    face_set_names[4] = "top";
    face_set_names[5] = "right";
    face_set_names[6] = "left";
    create_face_set_labels(face_set_names);
    for (auto it : face_set_names)
        set_face_set_name(it.first, it.second);
}

} // namespace godzilla
