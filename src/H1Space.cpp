#include "H1Space.h"
#include "CallStack.h"
#include "Set.h"
#include <assert.h>

namespace godzilla {

H1Space::H1Space(Mesh * mesh, Shapeset * ss) : Space(mesh, ss)
{
    _F_;
}

H1Space::~H1Space()
{
    _F_;
}

void
H1Space::assign_dofs_internal()
{
    _F_;
    Set vtx_init;

    FOR_ALL_ELEMENTS(elem_id, mesh)
    {
        const Element * elem = mesh->get_element(elem_id);
        for (uint i = 0; i < elem->get_num_vertices(); i++) {
            Index vtx_id = elem->get_vertex(i);
            VertexData * node_data = this->vertex_data[vtx_id];
            assert(node_data != NULL);
            if (!vtx_init.has(vtx_id)) {
                assign_vertex_dofs(node_data);
                vtx_init.set(vtx_id);
            }
        }

        // TODO: go over edges, if in 2D and 3D
        // TODO: go over faces, if in 3D

        ElementData * node_data = this->elem_data[elem_id];
        assign_bubble_dofs(node_data);
    }
}

uint
H1Space::get_vertex_ndofs()
{
    return 1;
}

uint
H1Space::get_edge_ndofs(uint order)
{
    assert(order >= 1);
    return order - 1;
}

uint
H1Space::get_face_ndofs(uint order)
{
    assert(order >= 1);

    // TODO: implement 2D and 3D versions

    // switch (mode) {
    // case MODE_TRIANGLE:
    //     return (order - 1) * (order - 2) / 2;
    // case MODE_QUAD:
    //     return (order - 1) * (order - 1);
    // default:
    //     error("Unknown mode");
    // }
    return 0;
}

uint
H1Space::get_element_ndofs(uint order)
{
    assert(order >= 1);

    // TODO: implement 2D and 3D versions

    // switch (order.type) {
    // case MODE_TETRAHEDRON:
    //     return (order - 1) * (order - 2) * (order - 3) / 6;
    // case MODE_HEXAHEDRON:
    //     return (order - 1) * (order - 1) * (order - 1);
    // default:
    //     error("Unknown mode");
    // }

    return order - 1;
}

} // namespace godzilla
