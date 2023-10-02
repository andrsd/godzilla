#include "Godzilla.h"
#include "RectangleMesh.h"
#include "CallStack.h"
#include "petscdmplex.h"
#include <array>

namespace godzilla {

REGISTER_OBJECT(RectangleMesh);

Parameters
RectangleMesh::parameters()
{
    Parameters params = UnstructuredMesh::parameters();
    params.add_param<Real>("xmin", 0., "Minimum in the x direction");
    params.add_param<Real>("xmax", 1., "Maximum in the x direction");
    params.add_param<Real>("ymin", 0., "Minimum in the y direction");
    params.add_param<Real>("ymax", 1., "Maximum in the y direction");
    params.add_required_param<Int>("nx", "Number of mesh points in the x direction");
    params.add_required_param<Int>("ny", "Number of mesh points in the y direction");
    params.add_param<bool>("simplex", false, "Generate simplex elements");
    return params;
}

RectangleMesh::RectangleMesh(const Parameters & parameters) :
    UnstructuredMesh(parameters),
    xmin(get_param<Real>("xmin")),
    xmax(get_param<Real>("xmax")),
    ymin(get_param<Real>("ymin")),
    ymax(get_param<Real>("ymax")),
    nx(get_param<Int>("nx")),
    ny(get_param<Int>("ny")),
    simplex(get_param<bool>("simplex") ? PETSC_TRUE : PETSC_FALSE),
    interpolate(PETSC_TRUE)
{
    _F_;
    if (this->xmax <= this->xmin)
        log_error("Parameter 'xmax' must be larger than 'xmin'.");
    if (this->ymax <= this->ymin)
        log_error("Parameter 'ymax' must be larger than 'ymin'.");
}

Real
RectangleMesh::get_x_min() const
{
    _F_;
    return this->xmin;
}

Real
RectangleMesh::get_x_max() const
{
    _F_;
    return this->xmax;
}

Int
RectangleMesh::get_nx() const
{
    _F_;
    return this->nx;
}

Real
RectangleMesh::get_y_min() const
{
    _F_;
    return this->ymin;
}

Real
RectangleMesh::get_y_max() const
{
    _F_;
    return this->ymax;
}

Int
RectangleMesh::get_ny() const
{
    _F_;
    return this->ny;
}

void
RectangleMesh::create_dm()
{
    _F_;
    std::array<Real, 2> lower = { this->xmin, this->ymin };
    std::array<Real, 2> upper = { this->xmax, this->ymax };
    std::array<Int, 2> faces = { this->nx, this->ny };
    std::array<DMBoundaryType, 2> periodicity = {
        this->simplex ? DM_BOUNDARY_NONE : DM_BOUNDARY_GHOSTED,
        this->simplex ? DM_BOUNDARY_NONE : DM_BOUNDARY_GHOSTED
    };

    PETSC_CHECK(DMPlexCreateBoxMesh(comm(),
                                    2,
                                    this->simplex,
                                    faces.data(),
                                    lower.data(),
                                    upper.data(),
                                    periodicity.data(),
                                    this->interpolate,
                                    &this->_dm));

    remove_label("marker");
    // create user-friendly names for sides
    std::map<Int, std::string> face_set_names;
    face_set_names[1] = "bottom";
    face_set_names[2] = "right";
    face_set_names[3] = "top";
    face_set_names[4] = "left";
    create_face_set_labels(face_set_names);
    for (auto it : face_set_names)
        set_face_set_name(it.first, it.second);
}

} // namespace godzilla
