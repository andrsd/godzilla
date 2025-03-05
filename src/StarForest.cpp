// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/StarForest.h"
#include "godzilla/CallStack.h"
#include "godzilla/Error.h"
#include <cassert>

namespace godzilla {

StarForest::Graph::Graph() : n_roots(0), n_leaves(0), leaves(nullptr), remote_leaves(nullptr) {}

StarForest::Graph::Graph(Int n_roots,
                         Int n_leaves,
                         const Int * leaves,
                         const PetscSFNode * remote_leaves) :
    n_roots(n_roots),
    n_leaves(n_leaves),
    leaves(leaves),
    remote_leaves(remote_leaves)
{
}

Int
StarForest::Graph::get_num_roots() const
{
    CALL_STACK_MSG();
    return this->n_roots;
}

Int
StarForest::Graph::get_num_leaves() const
{
    CALL_STACK_MSG();
    return this->n_leaves;
}

const Int *
StarForest::Graph::get_leaves() const
{
    CALL_STACK_MSG();
    return this->leaves;
}

Int
StarForest::Graph::get_leaf(Int idx) const
{
    CALL_STACK_MSG();
    return this->leaves[idx];
}

const StarForest::Node *
StarForest::Graph::get_remote_leaves() const
{
    CALL_STACK_MSG();
    return this->remote_leaves;
}

const StarForest::Node &
StarForest::Graph::get_remote_leaf(Int idx) const
{
    CALL_STACK_MSG();
    return this->remote_leaves[idx];
}

Int
StarForest::Graph::find_leaf(Int point) const
{
    CALL_STACK_MSG();
    assert(this->leaves != nullptr);
    Int idx;
    PETSC_CHECK(PetscFindInt(point, this->n_leaves, this->leaves, &idx));
    return idx;
}

StarForest::Graph::operator bool() const
{
    return this->leaves != nullptr;
}

StarForest::Graph::operator bool()
{
    return this->leaves != nullptr;
}

// ---

StarForest::StarForest() : sf(nullptr) {}

StarForest::StarForest(PetscSF sf) : sf(sf) {}

void
StarForest::create(MPI_Comm comm)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscSFCreate(comm, &this->sf));
}

void
StarForest::destroy()
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscSFDestroy(&this->sf));
}

void
StarForest::reset()
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscSFReset(this->sf));
}

void
StarForest::set_up()
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscSFSetUp(this->sf));
}

StarForest::Graph
StarForest::get_graph() const
{
    CALL_STACK_MSG();
    Int n_roots, n_leaves;
    const Int * leaves;
    const PetscSFNode * remote_leaves;
    PETSC_CHECK(PetscSFGetGraph(this->sf, &n_roots, &n_leaves, &leaves, &remote_leaves));
    return Graph(n_roots, n_leaves, leaves, remote_leaves);
}

void
StarForest::set_graph(Int n_roots,
                      Int n_leaves,
                      const std::vector<Int> & ilocal,
                      const std::vector<Node> & iremote) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscSFSetGraph(this->sf,
                                n_roots,
                                n_leaves,
                                const_cast<Int *>(ilocal.data()),
                                PETSC_COPY_VALUES,
                                const_cast<Node *>(iremote.data()),
                                PETSC_COPY_VALUES));
}

void
StarForest::view(PetscViewer viewer) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscSFView(this->sf, viewer));
}

StarForest::operator PetscSF() const
{
    CALL_STACK_MSG();
    return this->sf;
}

} // namespace godzilla
