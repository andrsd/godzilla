#include "H1Space.h"
#include "CallStack.h"
#include "Set.h"

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

    FOR_ALL_ELEMENTS(eid, mesh) {
        const Element * elem = mesh->get_element(eid);
        // for (uint ivtx = 0; ivtx < e->get_num_vertices(); ivtx++) {
        //     Index vid = elem->get_vertex(ivtx);
        //     VertexData * vd = this->vertex_data[vid];
        // }
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
    return order - 1;
}

uint
H1Space::get_face_ndofs(uint order)
{
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
    // switch (order.type) {
    // case MODE_TETRAHEDRON:
    //     return (order - 1) * (order - 2) * (order - 3) / 6;
    // case MODE_HEXAHEDRON:
    //     return (order - 1) * (order - 1) * (order - 1);
    // default:
    //     error("Unknown mode");
    // }
    return 0;
}

} // namespace godzilla
