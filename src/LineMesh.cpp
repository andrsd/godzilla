// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Godzilla.h"
#include "godzilla/LineMesh.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/CallStack.h"
#include "petscdm.h"
#include "petscdmplex.h"
#include <array>

namespace godzilla {

Parameters
LineMesh::parameters()
{
    Parameters params = MeshObject::parameters();
    params.add_param<Real>("xmin", 0., "Minimum in the x direction");
    params.add_param<Real>("xmax", 1., "Maximum in the x direction");
    params.add_required_param<Int>("nx", "Number of mesh points in the x direction");
    return params;
}

LineMesh::LineMesh(const Parameters & parameters) :
    MeshObject(parameters),
    xmin(get_param<Real>("xmin")),
    xmax(get_param<Real>("xmax")),
    nx(get_param<Int>("nx")),
    interpolate(PETSC_TRUE)
{
    CALL_STACK_MSG();
    if (this->xmax <= this->xmin)
        log_error("Parameter 'xmax' must be larger than 'xmin'.");
}

Real
LineMesh::get_x_min() const
{
    CALL_STACK_MSG();
    return this->xmin;
}

Real
LineMesh::get_x_max() const
{
    CALL_STACK_MSG();
    return this->xmax;
}

Int
LineMesh::get_nx() const
{
    CALL_STACK_MSG();
    return this->nx;
}

Mesh *
LineMesh::create_mesh()
{
    CALL_STACK_MSG();
    std::array<Real, 1> lower = { this->xmin };
    std::array<Real, 1> upper = { this->xmax };
    std::array<Int, 1> faces = { this->nx };
    std::array<DMBoundaryType, 1> periodicity = { DM_BOUNDARY_GHOSTED };

    DM dm;
#if PETSC_VERSION_GE(3, 22, 0)
    PETSC_CHECK(DMPlexCreateBoxMesh(get_comm(),
                                    1,
                                    PETSC_TRUE,
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
                                    1,
                                    PETSC_TRUE,
                                    faces.data(),
                                    lower.data(),
                                    upper.data(),
                                    periodicity.data(),
                                    this->interpolate,
                                    &dm));
#endif
    auto mesh = new UnstructuredMesh(dm);

    mesh->remove_label("marker");
    // create user-friendly names for sides
    std::map<Int, std::string> face_set_names;
    face_set_names[1] = "left";
    face_set_names[2] = "right";
    mesh->create_face_set_labels(face_set_names);
    for (auto [id, name] : face_set_names)
        mesh->set_face_set_name(id, name);

    return mesh;
}

} // namespace godzilla
