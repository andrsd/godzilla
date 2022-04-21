#include "Space.h"
#include "CallStack.h"

namespace godzilla {

Space::Space(Mesh * mesh, Shapeset * shapeset) : mesh(mesh), shapeset(shapeset)
{
    _F_;
    // TODO: check that shapeset is compatible with the space
    // init_data_tables();
}

Space::~Space()
{
    _F_;
    free_data_tables();
}

void
Space::set_uniform_order(uint order)
{
    _F_;
    FOR_ALL_ELEMENTS(eid, mesh)
    {
        const Element * elem = this->mesh->get_element(eid);
        this->elem_data[eid] = new ElementData(order);
    }
}

void
Space::assign_dofs(uint first_dof, uint stride)
{
    _F_;
    this->first_dof = this->next_dof = first_dof;
    this->stride = stride;
    free_data_tables();

    enforce_minimum_rule();
    set_bc_information();
    assign_dofs_internal();
}

void
Space::init_data_tables()
{
    _F_;
}

void
Space::free_data_tables()
{
    _F_;
    for (Index idx = this->vertex_data.first(); idx != INVALID_IDX;
         idx = this->vertex_data.next(idx))
        delete this->vertex_data[idx];
    this->vertex_data.remove_all();

    for (Index idx = this->edge_data.first(); idx != INVALID_IDX; idx = this->edge_data.next(idx))
        this->edge_data.remove_all();

    for (Index idx = this->face_data.first(); idx != INVALID_IDX; idx = this->face_data.next(idx))
        this->face_data.remove_all();

    for (Index idx = this->elem_data.first(); idx != INVALID_IDX; idx = this->elem_data.next(idx))
        this->elem_data.remove_all();
}

void
Space::set_bc_information()
{
    _F_;
}

void
Space::enforce_minimum_rule()
{
    _F_;
}

void
Space::assign_vertex_dofs(Index vid)
{
    _F_;
    VertexData * node = this->vertex_data[vid];
    uint ndofs = get_vertex_ndofs();
    if (node->bc_type == BC_ESSENTIAL) {
        node->dof = DIRICHLET_DOF;
    }
    else {
        node->dof = this->next_dof;
        this->next_dof += ndofs * this->stride;
    }
    node->n = ndofs;
}

void
Space::assign_edge_dofs(Index idx)
{
    _F_;
    EdgeData * node = this->edge_data[idx];
    uint ndofs = get_edge_ndofs(node->order);
    if (node->bc_type == BC_ESSENTIAL) {
        node->dof = DIRICHLET_DOF;
    }
    else {
        node->dof = this->next_dof;
        this->next_dof += ndofs * this->stride;
    }
    node->n = ndofs;
}

void
Space::assign_face_dofs(Index idx)
{
    _F_;
    FaceData * node = this->face_data[idx];
    int ndofs = get_face_ndofs(node->order);
    if (node->bc_type == BC_ESSENTIAL) {
        node->dof = DIRICHLET_DOF;
    }
    else {
        node->dof = this->next_dof;
        this->next_dof += ndofs * this->stride;
    }
    node->n = ndofs;
}

void
Space::assign_bubble_dofs(Index idx)
{
    _F_;
    ElementData * node = this->elem_data[idx];
    int ndofs = get_element_ndofs(node->order);
    node->n = ndofs;
    node->dof = this->next_dof;
    this->next_dof += ndofs * this->stride;
}

} // namespace godzilla
