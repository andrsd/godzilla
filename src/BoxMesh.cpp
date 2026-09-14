// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/BoxMesh.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/CallStack.h"
#include "petscdmplex.h"
#include <array>

namespace godzilla {

Parameters
BoxMesh::parameters()
{
    auto params = Object::parameters();
    params.add_param<Real>("xmin", 0., "Minimum in the x direction")
        .add_param<Real>("xmax", 1., "Maximum in the x direction")
        .add_param<Real>("ymin", 0., "Minimum in the y direction")
        .add_param<Real>("ymax", 1., "Maximum in the y direction")
        .add_param<Real>("zmin", 0., "Minimum in the z direction")
        .add_param<Real>("zmax", 1., "Maximum in the z direction")
        .add_required_param<Int>("nx", "Number of mesh points in the x direction")
        .add_required_param<Int>("ny", "Number of mesh points in the y direction")
        .add_required_param<Int>("nz", "Number of mesh points in the z direction")
        .add_param<bool>("simplex", false, "Generate simplex elements");
    return params;
}

BoxMesh::BoxMesh(const Parameters & pars) :
    Object(pars),
    xmin_(pars.get<Real>("xmin")),
    xmax_(pars.get<Real>("xmax")),
    ymin_(pars.get<Real>("ymin")),
    ymax_(pars.get<Real>("ymax")),
    zmin_(pars.get<Real>("zmin")),
    zmax_(pars.get<Real>("zmax")),
    nx_(pars.get<Int>("nx")),
    ny_(pars.get<Int>("ny")),
    nz_(pars.get<Int>("nz")),
    simplex_(pars.get<bool>("simplex")),
    interpolate_(true)
{
    CALL_STACK_MSG();
    expect_true(this->xmax_ > this->xmin_, "Parameter 'xmax' must be larger than 'xmin'.");
    expect_true(this->ymax_ > this->ymin_, "Parameter 'ymax' must be larger than 'ymin'.");
    expect_true(this->zmax_ > this->zmin_, "Parameter 'zmax' must be larger than 'zmin'.");
}

Real
BoxMesh::get_x_min() const
{
    CALL_STACK_MSG();
    return this->xmin_;
}

Real
BoxMesh::get_x_max() const
{
    CALL_STACK_MSG();
    return this->xmax_;
}

Int
BoxMesh::get_nx() const
{
    CALL_STACK_MSG();
    return this->nx_;
}

Real
BoxMesh::get_y_min() const
{
    CALL_STACK_MSG();
    return this->ymin_;
}

Real
BoxMesh::get_y_max() const
{
    CALL_STACK_MSG();
    return this->ymax_;
}

Int
BoxMesh::get_ny() const
{
    CALL_STACK_MSG();
    return this->ny_;
}

Real
BoxMesh::get_z_min() const
{
    CALL_STACK_MSG();
    return this->zmin_;
}

Real
BoxMesh::get_z_max() const
{
    CALL_STACK_MSG();
    return this->zmax_;
}

Int
BoxMesh::get_nz() const
{
    CALL_STACK_MSG();
    return this->nz_;
}

Qtr<UnstructuredMesh>
BoxMesh::create_mesh()
{
    CALL_STACK_MSG();
    std::array<Real, 3> lower = { this->xmin_, this->ymin_, this->zmin_ };
    std::array<Real, 3> upper = { this->xmax_, this->ymax_, this->zmax_ };
    std::array<Int, 3> faces = { this->nx_, this->ny_, this->nz_ };
    std::array<DMBoundaryType, 3> periodicity = {
        this->simplex_ ? DM_BOUNDARY_NONE : DM_BOUNDARY_GHOSTED,
        this->simplex_ ? DM_BOUNDARY_NONE : DM_BOUNDARY_GHOSTED,
        this->simplex_ ? DM_BOUNDARY_NONE : DM_BOUNDARY_GHOSTED
    };

    DM dm;
#if PETSC_VERSION_GE(3, 22, 0)
    PETSC_CHECK(DMPlexCreateBoxMesh(get_comm(),
                                    3,
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
                                    3,
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
    face_set_names[1] = "back";
    face_set_names[2] = "front";
    face_set_names[3] = "bottom";
    face_set_names[4] = "top";
    face_set_names[5] = "right";
    face_set_names[6] = "left";
    mesh->create_face_set_labels(face_set_names);
    for (auto & [id, name] : face_set_names)
        mesh->set_face_set_name(id, name);

    return mesh;
}

} // namespace godzilla
