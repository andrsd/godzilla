// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/StarForest.h"
#include "godzilla/CallStack.h"
#include "godzilla/Error.h"

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
    _F_;
    return this->n_roots;
}

Int
StarForest::Graph::get_num_leaves() const
{
    _F_;
    return this->n_leaves;
}

const Int *
StarForest::Graph::get_leaves() const
{
    _F_;
    return this->leaves;
}

const StarForest::Node *
StarForest::Graph::get_remote_leaves() const
{
    _F_;
    return this->remote_leaves;
}

Int
StarForest::Graph::find_leaf(Int point) const
{
    _F_;
    Int idx;
    PETSC_CHECK(PetscFindInt(point, this->n_leaves, this->leaves, &idx));
    return idx;
}

StarForest::Graph::operator bool() const
{
    return this->leaves != nullptr;
}

// ---

StarForest::StarForest() : sf(nullptr) {}

StarForest::StarForest(PetscSF sf) : sf(sf) {}

void
StarForest::create(MPI_Comm comm)
{
    _F_;
    PETSC_CHECK(PetscSFCreate(comm, &this->sf));
}

void
StarForest::destroy()
{
    _F_;
    PETSC_CHECK(PetscSFDestroy(&this->sf));
}

void
StarForest::reset()
{
    _F_;
    PETSC_CHECK(PetscSFReset(this->sf));
}

void
StarForest::set_up()
{
    _F_;
    PETSC_CHECK(PetscSFSetUp(this->sf));
}

StarForest::Graph
StarForest::get_graph() const
{
    _F_;
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
    _F_;
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
    _F_;
    PETSC_CHECK(PetscSFView(this->sf, viewer));
}

StarForest::operator PetscSF() const
{
    _F_;
    return this->sf;
}

} // namespace godzilla
