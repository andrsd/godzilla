// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/RectangleMesh.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/CallStack.h"
#include "petscdmplex.h"
#include <array>

namespace godzilla {

Parameters
RectangleMesh::parameters()
{
    auto params = MeshObject::parameters();
    params.add_param<Real>("xmin", 0., "Minimum in the x direction")
        .add_param<Real>("xmax", 1., "Maximum in the x direction")
        .add_param<Real>("ymin", 0., "Minimum in the y direction")
        .add_param<Real>("ymax", 1., "Maximum in the y direction")
        .add_required_param<Int>("nx", "Number of mesh points in the x direction")
        .add_required_param<Int>("ny", "Number of mesh points in the y direction")
        .add_param<bool>("simplex", false, "Generate simplex elements");
    return params;
}

RectangleMesh::RectangleMesh(const Parameters & parameters) :
    MeshObject(parameters),
    xmin(get_param<Real>("xmin")),
    xmax(get_param<Real>("xmax")),
    ymin(get_param<Real>("ymin")),
    ymax(get_param<Real>("ymax")),
    nx(get_param<Int>("nx")),
    ny(get_param<Int>("ny")),
    simplex(get_param<bool>("simplex") ? PETSC_TRUE : PETSC_FALSE),
    interpolate(PETSC_TRUE)
{
    CALL_STACK_MSG();
    if (this->xmax <= this->xmin)
        log_error("Parameter 'xmax' must be larger than 'xmin'.");
    if (this->ymax <= this->ymin)
        log_error("Parameter 'ymax' must be larger than 'ymin'.");
}

Real
RectangleMesh::get_x_min() const
{
    CALL_STACK_MSG();
    return this->xmin;
}

Real
RectangleMesh::get_x_max() const
{
    CALL_STACK_MSG();
    return this->xmax;
}

Int
RectangleMesh::get_nx() const
{
    CALL_STACK_MSG();
    return this->nx;
}

Real
RectangleMesh::get_y_min() const
{
    CALL_STACK_MSG();
    return this->ymin;
}

Real
RectangleMesh::get_y_max() const
{
    CALL_STACK_MSG();
    return this->ymax;
}

Int
RectangleMesh::get_ny() const
{
    CALL_STACK_MSG();
    return this->ny;
}

godzilla::Mesh *
RectangleMesh::create_mesh()
{
    CALL_STACK_MSG();
    std::array<Real, 2> lower = { this->xmin, this->ymin };
    std::array<Real, 2> upper = { this->xmax, this->ymax };
    std::array<Int, 2> faces = { this->nx, this->ny };
    std::array<DMBoundaryType, 2> periodicity = {
        this->simplex ? DM_BOUNDARY_NONE : DM_BOUNDARY_GHOSTED,
        this->simplex ? DM_BOUNDARY_NONE : DM_BOUNDARY_GHOSTED
    };

    DM dm;
#if PETSC_VERSION_GE(3, 22, 0)
    PETSC_CHECK(DMPlexCreateBoxMesh(get_comm(),
                                    2,
                                    this->simplex,
                                    faces.data(),
                                    lower.data(),
                                    upper.data(),
                                    periodicity.data(),
                                    this->interpolate,
                                    0,
                                    PETSC_FALSE,
                                    &dm));
#else
    PETSC_CHECK(DMPlexCreateBoxMesh(get_comm(),
                                    2,
                                    this->simplex,
                                    faces.data(),
                                    lower.data(),
                                    upper.data(),
                                    periodicity.data(),
                                    this->interpolate,
                                    &dm));
#endif
    auto * mesh = new UnstructuredMesh(dm);

    mesh->remove_label("marker");
    // create user-friendly names for sides
    std::map<Int, std::string> face_set_names;
    face_set_names[1] = "bottom";
    face_set_names[2] = "right";
    face_set_names[3] = "top";
    face_set_names[4] = "left";
    mesh->create_face_set_labels(face_set_names);
    for (auto [id, name] : face_set_names)
        mesh->set_face_set_name(id, name);

    return mesh;
}

} // namespace godzilla
