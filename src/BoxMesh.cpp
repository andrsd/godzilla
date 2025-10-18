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
    xmin(pars.get<Real>("xmin")),
    xmax(pars.get<Real>("xmax")),
    ymin(pars.get<Real>("ymin")),
    ymax(pars.get<Real>("ymax")),
    zmin(pars.get<Real>("zmin")),
    zmax(pars.get<Real>("zmax")),
    nx(pars.get<Int>("nx")),
    ny(pars.get<Int>("ny")),
    nz(pars.get<Int>("nz")),
    simplex(pars.get<bool>("simplex") ? PETSC_TRUE : PETSC_FALSE),
    interpolate(PETSC_TRUE)
{
    CALL_STACK_MSG();
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
    CALL_STACK_MSG();
    return this->xmin;
}

Real
BoxMesh::get_x_max() const
{
    CALL_STACK_MSG();
    return this->xmax;
}

Int
BoxMesh::get_nx() const
{
    CALL_STACK_MSG();
    return this->nx;
}

Real
BoxMesh::get_y_min() const
{
    CALL_STACK_MSG();
    return this->ymin;
}

Real
BoxMesh::get_y_max() const
{
    CALL_STACK_MSG();
    return this->ymax;
}

Int
BoxMesh::get_ny() const
{
    CALL_STACK_MSG();
    return this->ny;
}

Real
BoxMesh::get_z_min() const
{
    CALL_STACK_MSG();
    return this->zmin;
}

Real
BoxMesh::get_z_max() const
{
    CALL_STACK_MSG();
    return this->zmax;
}

Int
BoxMesh::get_nz() const
{
    CALL_STACK_MSG();
    return this->nz;
}

Qtr<UnstructuredMesh>
BoxMesh::create_mesh()
{
    CALL_STACK_MSG();
    std::array<Real, 3> lower = { this->xmin, this->ymin, this->zmin };
    std::array<Real, 3> upper = { this->xmax, this->ymax, this->zmax };
    std::array<Int, 3> faces = { this->nx, this->ny, this->nz };
    std::array<DMBoundaryType, 3> periodicity = {
        this->simplex ? DM_BOUNDARY_NONE : DM_BOUNDARY_GHOSTED,
        this->simplex ? DM_BOUNDARY_NONE : DM_BOUNDARY_GHOSTED,
        this->simplex ? DM_BOUNDARY_NONE : DM_BOUNDARY_GHOSTED
    };

    DM dm;
#if PETSC_VERSION_GE(3, 22, 0)
    PETSC_CHECK(DMPlexCreateBoxMesh(get_comm(),
                                    3,
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
                                    3,
                                    this->simplex,
                                    faces.data(),
                                    lower.data(),
                                    upper.data(),
                                    periodicity.data(),
                                    this->interpolate,
                                    &dm));
#endif
    auto mesh = Qtr<UnstructuredMesh>::alloc(dm);

    mesh->remove_label("marker");
    // create user-friendly names for sides
    std::map<Int, std::string> face_set_names;
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
