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
    auto params = Object::parameters();
    params.add_param<Real>("xmin", 0., "Minimum in the x direction")
        .add_param<Real>("xmax", 1., "Maximum in the x direction")
        .add_param<Real>("ymin", 0., "Minimum in the y direction")
        .add_param<Real>("ymax", 1., "Maximum in the y direction")
        .add_required_param<Int>("nx", "Number of mesh points in the x direction")
        .add_required_param<Int>("ny", "Number of mesh points in the y direction")
        .add_param<bool>("simplex", false, "Generate simplex elements");
    return params;
}

RectangleMesh::RectangleMesh(const Parameters & pars) :
    Object(pars),
    xmin_(pars.get<Real>("xmin")),
    xmax_(pars.get<Real>("xmax")),
    ymin_(pars.get<Real>("ymin")),
    ymax_(pars.get<Real>("ymax")),
    nx_(pars.get<Int>("nx")),
    ny_(pars.get<Int>("ny")),
    simplex_(pars.get<bool>("simplex")),
    interpolate_(true)
{
    CALL_STACK_MSG();
    expect_true(this->xmax_ > this->xmin_, "Parameter 'xmax' must be larger than 'xmin'.");
    expect_true(this->ymax_ > this->ymin_, "Parameter 'ymax' must be larger than 'ymin'.");
}

Real
RectangleMesh::get_x_min() const
{
    CALL_STACK_MSG();
    return this->xmin_;
}

Real
RectangleMesh::get_x_max() const
{
    CALL_STACK_MSG();
    return this->xmax_;
}

Int
RectangleMesh::get_nx() const
{
    CALL_STACK_MSG();
    return this->nx_;
}

Real
RectangleMesh::get_y_min() const
{
    CALL_STACK_MSG();
    return this->ymin_;
}

Real
RectangleMesh::get_y_max() const
{
    CALL_STACK_MSG();
    return this->ymax_;
}

Int
RectangleMesh::get_ny() const
{
    CALL_STACK_MSG();
    return this->ny_;
}

Qtr<UnstructuredMesh>
RectangleMesh::create_mesh()
{
    CALL_STACK_MSG();
    std::array<Real, 2> lower = { this->xmin_, this->ymin_ };
    std::array<Real, 2> upper = { this->xmax_, this->ymax_ };
    std::array<Int, 2> faces = { this->nx_, this->ny_ };
    std::array<DMBoundaryType, 2> periodicity = {
        this->simplex_ ? DM_BOUNDARY_NONE : DM_BOUNDARY_GHOSTED,
        this->simplex_ ? DM_BOUNDARY_NONE : DM_BOUNDARY_GHOSTED
    };

    DM dm;
#if PETSC_VERSION_GE(3, 22, 0)
    PETSC_CHECK(DMPlexCreateBoxMesh(get_comm(),
                                    2,
                                    this->simplex_ ? PETSC_TRUE : PETSC_FALSE,
                                    faces.data(),
                                    lower.data(),
                                    upper.data(),
                                    periodicity.data(),
                                    this->interpolate_ ? PETSC_TRUE : PETSC_FALSE,
                                    0,
                                    PETSC_FALSE,
                                    &dm));
#else
    PETSC_CHECK(DMPlexCreateBoxMesh(get_comm(),
                                    2,
                                    this->simplex ? PETSC_TRUE : PETSC_FALSE,
                                    faces.data(),
                                    lower.data(),
                                    upper.data(),
                                    periodicity.data(),
                                    this->interpolate ? PETSC_TRUE : PETSC_FALSE,
                                    &dm));
#endif
    auto mesh = Qtr<UnstructuredMesh>::alloc(dm);

    mesh->remove_label("marker");
    // create user-friendly names for sides
    std::map<Int, String> face_set_names;
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
