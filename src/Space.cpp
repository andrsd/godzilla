#include "Space.h"
#include "CallStack.h"
#include "BoundaryCondition.h"
#include <assert.h>
#include <iostream>

namespace godzilla {

Space::Space(Mesh * mesh, Shapeset * shapeset) : mesh(mesh), shapeset(shapeset)
{
    _F_;
    // TODO: check that shapeset is compatible with the space
    // init_data_tables();
    this->first_dof = this->next_dof = 0;
    this->stride = 1;
}

Space::~Space()
{
    _F_;
    free_data_tables();
}

const Shapeset *
Space::get_shapeset() const
{
    _F_;
    return this->shapeset;
}

void
Space::set_uniform_order(uint order)
{
    _F_;
    for (auto & elem : this->mesh->get_elements()) {
        PetscInt eid = elem->get_id();
        this->elem_data[eid] = new ElementData(order);
    }
}

void
Space::assign_dofs(uint first_dof, uint stride)
{
    _F_;
    this->first_dof = this->next_dof = first_dof;
    this->stride = stride;

    enforce_minimum_rule();
    set_bc_information();
    assign_dofs_internal();
    update_constraints();
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
Space::add_boundary_condition(BoundaryCondition * bc)
{
    _F_;

    const std::string & bnd_name = bc->get_boundary_name();
    uint marker = this->mesh->get_marker_by_name(bnd_name);
    if (!this->marker_to_bcs.exists(marker)) {
        this->marker_to_bcs[marker] = bc;

        PetscErrorCode ierr;
        DM dm = this->mesh->get_dm();
        IS is;
        PetscInt n_ids;
        const PetscInt * ids;

        DMLabel label;
        ierr = DMGetLabel(dm, bnd_name.c_str(), &label);
        checkPetscError(ierr);

        ierr = DMLabelGetValueIS(label, &is);
        checkPetscError(ierr);
        ierr = ISGetSize(is, &n_ids);
        assert(n_ids == 1);
        checkPetscError(ierr);
        ierr = ISGetIndices(is, &ids);
        checkPetscError(ierr);
        PetscInt bnd_stratum = ids[0];
        ierr = ISRestoreIndices(is, &ids);
        checkPetscError(ierr);
        ierr = ISDestroy(&is);
        checkPetscError(ierr);

        ierr = DMLabelGetStratumIS(label, bnd_stratum, &is);
        checkPetscError(ierr);
        ierr = ISGetSize(is, &n_ids);
        checkPetscError(ierr);
        ierr = ISGetIndices(is, &ids);
        checkPetscError(ierr);

        for (PetscInt i = 0; i < n_ids; i++) {
            BoundaryInfo * info = new BoundaryInfo(ids[i], marker);
            this->side_boundaries.add(info);
        }

        ierr = ISRestoreIndices(is, &ids);
        checkPetscError(ierr);
        ierr = ISDestroy(&is);
        checkPetscError(ierr);
    }
    else
        // TODO: add variable/field name into the error message
        error("Unable to add boundary condition '",
              bc->get_name(),
              "'. Boundary condition on boundary '",
              bnd_name,
              "' already exists.");
}

void
Space::set_bc_information()
{
    _F_;
    for (auto & bnd : this->side_boundaries) {
        const uint & marker = bnd->marker;

        const BoundaryCondition * bc = this->marker_to_bcs[marker];
        assert(bc != nullptr);

        // 1D
        Index vtx_idx = bnd->id;
        assert(this->vertex_data.exists(vtx_idx));
        set_bc_info(this->vertex_data[vtx_idx], bc->get_bc_type(), marker);

        // TODO: handle 2D
        // TODO: handle 3D
    }
}

void
Space::set_bc_info(NodeData * node, BoundaryConditionType bc_type, uint marker)
{
    _F_;
    if (bc_type == BC_ESSENTIAL || (bc_type == BC_NATURAL && node->bc_type == BC_NONE)) {
        node->bc_type = bc_type;
        node->marker = marker;
    }
}

void
Space::update_constraints()
{
    _F_;
    for (auto & bnd : this->side_boundaries) {
        // 1D
        calc_vertex_boundary_projection(bnd->id);

        // TODO: handle 2D
        // TODO: handle 3D
    }
}

void
Space::enforce_minimum_rule()
{
    _F_;
    for (auto * vtx : this->mesh->get_vertices()) {
        VertexData * node = new VertexData();
        MEM_CHECK(node);
        this->vertex_data[vtx->id] = node;
    }
}

void
Space::assign_vertex_dofs(PetscInt vertex_id)
{
    _F_;
    VertexData * node = this->vertex_data[vertex_id];
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
Space::assign_edge_dofs(EdgeData * node)
{
    _F_;
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
Space::assign_face_dofs(FaceData * node)
{
    _F_;
    uint ndofs = get_face_ndofs(node->order);
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
Space::assign_bubble_dofs(PetscInt elem_id)
{
    _F_;
    ElementData * node = this->elem_data[elem_id];
    uint ndofs = get_element_ndofs(node->order);
    node->n = ndofs;
    node->dof = this->next_dof;
    this->next_dof += ndofs * this->stride;
}

void
Space::get_vertex_assembly_list(const Element * e, uint ivertex, AssemblyList * al)
{
    _F_;
    Index vtx_id = e->get_vertex(ivertex);
    VertexData * vnode = this->vertex_data[vtx_id];
    uint index = shapeset->get_vertex_index(ivertex);
    Scalar coef = vnode->dof == DIRICHLET_DOF ? vnode->bc_proj : 1.0;
    assert(vnode->dof == DIRICHLET_DOF ||
           (vnode->dof >= this->first_dof && vnode->dof < this->next_dof));
    al->add(index, vnode->dof, coef);
}

void
Space::get_edge_assembly_list(const Element * elem, uint iedge, AssemblyList * al)
{
    _F_;
    Index edge_id = mesh->get_edge_id(elem, iedge);
    EdgeData * enode = this->edge_data[edge_id];
    uint ori = elem->get_edge_orientation(iedge);
    if (enode->n > 0) {
        uint * indices = shapeset->get_edge_indices(iedge, ori, enode->order);
        if (enode->dof != DIRICHLET_DOF) {
            for (uint j = 0, dof = enode->dof; j < enode->n; j++, dof += this->stride) {
                assert(dof >= this->first_dof && dof < this->next_dof);
                al->add(indices[j], dof, 1.0);
            }
        }
        else if (enode->bc_proj != NULL) {
            for (uint j = 0; j < enode->n; j++) {
                Scalar coef = enode->bc_proj[j];
                al->add(indices[j], DIRICHLET_DOF, coef);
            }
        }
    }
}

void
Space::get_face_assembly_list(const Element * elem, uint iface, AssemblyList * al)
{
    _F_;
    Index face_id = mesh->get_face_id(elem, iface);
    FaceData * fnode = this->face_data[face_id];
    uint ori = elem->get_face_orientation(iface);
    if (fnode->n > 0) {
        uint * indices = shapeset->get_face_indices(iface, ori, fnode->order);
        if (fnode->dof != DIRICHLET_DOF) {
            for (uint j = 0, dof = fnode->dof; j < fnode->n; j++, dof += this->stride) {
                assert(dof >= this->first_dof && dof < this->next_dof);
                al->add(indices[j], dof, 1.0);
            }
        }
        else if (fnode->bc_proj != NULL) {
            for (uint j = 0; j < fnode->n; j++) {
                Scalar coef = fnode->bc_proj[j];
                al->add(indices[j], DIRICHLET_DOF, coef);
            }
        }
    }
}

void
Space::get_bubble_assembly_list(const Element * e, AssemblyList * al)
{
    _F_;
    ElementData * enode = this->elem_data[e->get_id()];
    if (enode->n > 0) {
        uint * indices = shapeset->get_bubble_indices(enode->order);
        for (uint j = 0, dof = enode->dof; j < enode->n; j++, dof += this->stride) {
            assert(dof >= this->first_dof && dof < this->next_dof);
            al->add(indices[j], dof, 1.0);
        }
    }
}

} // namespace godzilla
