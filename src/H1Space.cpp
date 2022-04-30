#include "H1Space.h"
#include "CallStack.h"
#include "Set.h"
#include "EssentialBC.h"
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
    for (auto & vtx : this->mesh->get_vertices())
        assign_vertex_dofs(vtx->id);
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

void
H1Space::get_element_assembly_list(const Element * e, AssemblyList * al)
{
    _F_;
    al->clear();
    for (uint i = 0; i < e->get_num_vertices(); i++)
        get_vertex_assembly_list(e, i, al);
    // for (uint i = 0; i < e->get_num_edges(); i++)
    //     get_edge_assembly_list(e, i, al);
    // for (uint i = 0; i < e->get_num_faces(); i++)
    //     get_face_assembly_list(e, i, al);
    // get_bubble_assembly_list(e, al);
}

void
H1Space::calc_vertex_boundary_projection(Index vtx_idx)
{
    _F_;
    assert(this->vertex_data.exists(vtx_idx));
    VertexData * vnode = this->vertex_data[vtx_idx];
    const Vertex * vertex = this->mesh->get_vertex(vtx_idx);
    if (vnode->bc_type == BC_ESSENTIAL) {
        /// FIXME: would be nice if we did not have to do this downcast
        const Vertex1D * v = static_cast<const Vertex1D *>(vertex);
        /// FIXME: remove this down_cast
        EssentialBC * bc =
            dynamic_cast<EssentialBC *>(this->marker_to_bcs[vnode->marker]);
        assert(bc != nullptr);
        vnode->bc_proj = bc->evaluate(0., v->x, 0., 0.);

        // TODO: handle 2D and 3D case
    }
}

void
H1Space::calc_edge_boundary_projection(const Element * elem, uint iedge)
{
    _F_;
    error("Not implemented");
}

void
H1Space::calc_face_boundary_projection(const Element * elem, uint iface)
{
    _F_;
    error("Not implemented");
}

} // namespace godzilla
