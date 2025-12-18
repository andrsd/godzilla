// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/NetworkMesh.h"
#include "godzilla/CallStack.h"
#include "petscdmnetwork.h"

namespace godzilla {

MeshNetwork::MeshNetwork(mpi::Communicator comm) : godzilla::Mesh()
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMNetworkCreate(comm, &this->netw_));
    set_dm(this->netw_);
}

MeshNetwork::MeshNetwork(DM dm) : godzilla::Mesh(dm)
{
    CALL_STACK_MSG();
}

Int
MeshNetwork::register_component(String name, std::size_t size)
{
    CALL_STACK_MSG();
    Int key;
    PETSC_CHECK(DMNetworkRegisterComponent(this->netw_, name.c_str(), size, &key));
    return key;
}

void
MeshNetwork::add_component(Int p, Int key, void * comp, Int n_vars)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMNetworkAddComponent(this->netw_, p, key, comp, n_vars));
}

void *
MeshNetwork::component(Int p)
{
    CALL_STACK_MSG();
    void * ptr;
    PETSC_CHECK(DMNetworkGetComponent(this->netw_, p, 0, NULL, &ptr, NULL));
    return ptr;
}

std::tuple<Int, Int>
MeshNetwork::num_sub_networks() const
{
    CALL_STACK_MSG();
    Int l, g;
    PETSC_CHECK(DMNetworkGetNumSubNetworks(this->netw_, &l, &g));
    return { l, g };
}

void
MeshNetwork::set_num_sub_networks(Int n, Int N)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMNetworkSetNumSubNetworks(this->netw_, n, N));
}

Int
MeshNetwork::add_sub_network(String name, std::vector<Int> & edge_list)
{
    CALL_STACK_MSG();
    if (edge_list.size() % 2 == 0) {
        Int n = edge_list.size() / 2;
        Int num;
        PETSC_CHECK(DMNetworkAddSubnetwork(this->netw_, NULL, n, edge_list.data(), &num));
        return num;
    }
    else
        throw Exception("Edge list size ({}) must be divisible by 2", edge_list.size());
}

void
MeshNetwork::create()
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMNetworkCreate(get_comm(), &this->netw_));
}

int
MeshNetwork::num_components(Int p) const
{
    CALL_STACK_MSG();
    Int n;
    PETSC_CHECK(DMNetworkGetNumComponents(this->netw_, p, &n));
    return n;
}

std::tuple<Int, Int>
MeshNetwork::num_vertices() const
{
    CALL_STACK_MSG();
    Int loc, glob;
    PETSC_CHECK(DMNetworkGetNumVertices(this->netw_, &loc, &glob));
    return { loc, glob };
}

Range
MeshNetwork::vertex_range() const
{
    CALL_STACK_MSG();
    Int start, end;
    PETSC_CHECK(DMNetworkGetVertexRange(this->netw_, &start, &end));
    return { start, end };
}

std::vector<Int>
MeshNetwork::supporting_edges(Int vertex)
{
    CALL_STACK_MSG();
    Int n_edges;
    const Int * edges;
    PETSC_CHECK(DMNetworkGetSupportingEdges(this->netw_, vertex, &n_edges, &edges));
    return std::vector<Int>(edges, edges + n_edges);
}

bool
MeshNetwork::is_ghost_vertex(Int p) const
{
    CALL_STACK_MSG();
    PetscBool isghost;
    PETSC_CHECK(DMNetworkIsGhostVertex(this->netw_, p, &isghost));
    return isghost == PETSC_TRUE;
}

std::tuple<Int, Int>
MeshNetwork::num_edges() const
{
    CALL_STACK_MSG();
    Int loc, glob;
    PETSC_CHECK(DMNetworkGetNumEdges(this->netw_, &loc, &glob));
    return { loc, glob };
}

Range
MeshNetwork::edge_range() const
{
    CALL_STACK_MSG();
    Int start, end;
    PETSC_CHECK(DMNetworkGetEdgeRange(this->netw_, &start, &end));
    return { start, end };
}

std::array<Int, 2>
MeshNetwork::connected_vertices(Int edge)
{
    CALL_STACK_MSG();
    const Int * verts;
    PETSC_CHECK(DMNetworkGetConnectedVertices(this->netw_, edge, &verts));
    return { verts[0], verts[1] };
}

void
MeshNetwork::layout_set_up()
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMNetworkLayoutSetUp(this->netw_));
}

void
MeshNetwork::distribute(Int overlap)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMNetworkDistribute(&this->netw_, overlap));
}

void
MeshNetwork::finalize_components()
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMNetworkFinalizeComponents(this->netw_));
}

Int
MeshNetwork::edge_offset(Int p) const
{
    CALL_STACK_MSG();
    Int offset;
    PETSC_CHECK(DMNetworkGetEdgeOffset(this->netw_, p, &offset));
    return offset;
}

Int
MeshNetwork::global_edge_index(Int p) const
{
    CALL_STACK_MSG();
    Int idx;
    PETSC_CHECK(DMNetworkGetGlobalEdgeIndex(this->netw_, p, &idx));
    return idx;
}

Int
MeshNetwork::global_vec_offset(Int p, Int comp_num) const
{
    CALL_STACK_MSG();
    Int offset;
    PETSC_CHECK(DMNetworkGetGlobalVecOffset(this->netw_, p, comp_num, &offset));
    return offset;
}

Int
MeshNetwork::global_vertex_index(Int p) const
{
    CALL_STACK_MSG();
    Int idx;
    PETSC_CHECK(DMNetworkGetGlobalVertexIndex(this->netw_, p, &idx));
    return idx;
}

Int
MeshNetwork::local_vec_offset(Int p, Int comp_num) const
{
    CALL_STACK_MSG();
    Int offset;
    PETSC_CHECK(DMNetworkGetLocalVecOffset(this->netw_, p, comp_num, &offset));
    return offset;
}

Int
MeshNetwork::vertex_offset(Int p) const
{
    CALL_STACK_MSG();
    Int offset;
    PETSC_CHECK(DMNetworkGetVertexOffset(this->netw_, p, &offset));
    return offset;
}

void
MeshNetwork::edge_set_matrix(Int p, const std::array<Matrix, 3> & J)
{
    CALL_STACK_MSG();
    Mat Jarr[3] = { J[0], J[1], J[2] };
    PETSC_CHECK(DMNetworkEdgeSetMatrix(this->netw_, p, Jarr));
}

void
MeshNetwork::vertex_set_matrix(Int p, const std::vector<Matrix> & J)
{
    CALL_STACK_MSG();
    std::vector<Mat> Jarr(J.size());
    for (std::size_t i = 0; i < J.size(); ++i)
        Jarr[i] = J[i];
    PETSC_CHECK(DMNetworkVertexSetMatrix(this->netw_, p, Jarr.data()));
}

void
MeshNetwork::set_user_jacobian(bool eflg, bool vflg)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMNetworkHasJacobian(this->netw_,
                                     eflg ? PETSC_TRUE : PETSC_FALSE,
                                     vflg ? PETSC_TRUE : PETSC_FALSE));
}

} // namespace godzilla
