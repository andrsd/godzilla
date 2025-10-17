// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/MeshObject.h"
#include "godzilla/App.h"
#include "godzilla/Mesh.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/CallStack.h"
#include "godzilla/Utils.h"

namespace godzilla {

Parameters
MeshObject::parameters()
{
    auto params = Object::parameters();
    return params;
}

MeshObject::MeshObject(const Parameters & pars) : Object(pars), PrintInterface(this) {}

void
MeshObject::create()
{
    CALL_STACK_MSG();
    this->mesh = create_mesh();
    if (this->mesh) {
        this->mesh->set_up();
        lprint_mesh_info();
    }
}

void
MeshObject::lprint_mesh_info()
{
    CALL_STACK_MSG();
    if (get_mesh<UnstructuredMesh>()) {
        auto um = get_mesh<UnstructuredMesh>();
        lprintln(9, "Information:");
        lprintln(9, "- vertices: {}", utils::human_number(um->get_num_vertices()));
        lprintln(9, "- elements: {}", utils::human_number(um->get_num_cells()));
    }
}

} // namespace godzilla
