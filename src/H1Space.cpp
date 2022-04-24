#include "H1Space.h"
#include "CallStack.h"
#include "Set.h"
#include <assert.h>

namespace godzilla {

static Scalar
default_bc_value_by_coord(uint marker, double x, double y, double z)
{
    return 0.0;
}

H1Space::H1Space(Mesh * mesh, Shapeset * ss) : Space(mesh, ss)
{
    _F_;
    this->bc_value_callback_by_coord = default_bc_value_by_coord;
}

H1Space::~H1Space()
{
    _F_;
}

void H1Space::set_bc_values(Scalar (*fn)(uint, double, double, double))
{
    _F_;
    if (fn == nullptr)
        this->bc_value_callback_by_coord = default_bc_value_by_coord;
    else
        this->bc_value_callback_by_coord = fn;
}

void
H1Space::assign_dofs_internal()
{
    _F_;
    Set vtx_init;

    // FOR_ALL_ELEMENTS(elem_id, mesh)
    // {
    //     const Element * elem = mesh->get_element(elem_id);
    //     for (uint i = 0; i < elem->get_num_vertices(); i++) {
    //         Index vtx_id = elem->get_vertex(i);
    //         VertexData * node_data = this->vertex_data[vtx_id];
    //         assert(node_data != NULL);
    //         if (!vtx_init.has(vtx_id)) {
    //             assign_vertex_dofs(node_data);
    //             vtx_init.set(vtx_id);
    //         }
    //     }
    //
    //     // TODO: go over edges, if in 2D and 3D
    //     // TODO: go over faces, if in 3D
    //
    //     ElementData * node_data = this->elem_data[elem_id];
    //     assign_bubble_dofs(node_data);
    // }
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
H1Space::set_vertex_bc_info(PetscInt vertex_id)
{
    _F_;
    assert(this->vertex_data.exists(vertex_id));
    VertexData * vnode = this->vertex_data[vertex_id];
    const Vertex * vertex = this->mesh->get_vertex(vertex_id);
    if (vnode->bc_type == BC_ESSENTIAL) {
        /// FIXME: would be nice if we did not have to do this downcast
        const Vertex1D * v = static_cast<const Vertex1D *>(vertex);
        vnode->bc_proj = bc_value_callback_by_coord(vnode->marker, v->x, 0., 0.);
        // TODO: handle 2D and 3D case
    }
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
H1Space::calc_vertex_boundary_projection(const Element * elem, uint ivertex)
{
    _F_;
    Index vtx_idx = this->mesh->get_vertex_id(elem, ivertex);
    assert(this->vertex_data.exists(vtx_idx));
    VertexData * vnode = this->vertex_data[vtx_idx];
    const Vertex * vertex = this->mesh->get_vertex(vtx_idx);
    if (vnode->bc_type == BC_ESSENTIAL) {
        /// FIXME: would be nice if we did not have to do this downcast
        const Vertex1D * v = static_cast<const Vertex1D *>(vertex);
        vnode->bc_proj = bc_value_callback_by_coord(vnode->marker, v->x, 0., 0.);
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
