// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Partitioning.h"
#include "godzilla/CallStack.h"
#include <petscmat.h>

namespace godzilla {

const char * Partitioning::AVERAGE = MATPARTITIONINGAVERAGE;
const char * Partitioning::SQUARE = MATPARTITIONINGSQUARE;
const char * Partitioning::PARMETIS = MATPARTITIONINGPARMETIS;
const char * Partitioning::PARTY = MATPARTITIONINGPARTY;
const char * Partitioning::PTSCOTCH = MATPARTITIONINGPTSCOTCH;

Partitioning::Partitioning() : PetscObjectWrapper(nullptr) {}

Partitioning::Partitioning(MatPartitioning mp) : PetscObjectWrapper(mp) {}

void
Partitioning::create(MPI_Comm comm)
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatPartitioningCreate(comm, &this->obj));
}

void
Partitioning::destroy()
{
    CALL_STACK_MSG();
}

String
Partitioning::get_type() const
{
    CALL_STACK_MSG();
    MatPartitioningType type;
    PETSC_CHECK(MatPartitioningGetType(this->obj, &type));
    return { type };
}

void
Partitioning::set_type(String type) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatPartitioningSetType(this->obj, type.c_str()));
}

void
Partitioning::set_n_parts(Int n)
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatPartitioningSetNParts(this->obj, n));
}

bool
Partitioning::get_use_edge_weights() const
{
    CALL_STACK_MSG();
    PetscBool flag;
    PETSC_CHECK(MatPartitioningGetUseEdgeWeights(this->obj, &flag));
    return flag == PETSC_TRUE;
}

void
Partitioning::set_use_edge_weights(bool flag)
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatPartitioningSetUseEdgeWeights(this->obj, flag ? PETSC_TRUE : PETSC_FALSE));
}

IndexSet
Partitioning::apply()
{
    CALL_STACK_MSG();
    IS is;
    PETSC_CHECK(MatPartitioningApply(this->obj, &is));
    return IndexSet(is);
}

void
Partitioning::improve(IndexSet & partitioning)
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatPartitioningImprove(this->obj, partitioning));
}

void
Partitioning::set_adjacency(const Matrix & adj)
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatPartitioningSetAdjacency(this->obj, adj));
}

void
Partitioning::set_number_vertex_weights(Int n)
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatPartitioningSetNumberVertexWeights(this->obj, n));
}

void
Partitioning::set_vertex_weights(const std::vector<Int> & weights)
{
    CALL_STACK_MSG();
    // @note This is not a mistake, see doco
    PetscInt * wts;
    PetscMalloc(sizeof(PetscInt) * weights.size(), &wts);
    for (std::size_t i = 0; i < weights.size(); ++i)
        wts[i] = weights[i];
    PETSC_CHECK(MatPartitioningSetVertexWeights(this->obj, wts));
}

void
Partitioning::set_vertex_weights(const Int weights[])
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatPartitioningSetVertexWeights(this->obj, weights));
}

void
Partitioning::set_partition_weights(const std::vector<Real> & weights)
{
    CALL_STACK_MSG();
    // @note This is not a mistake, see doco
    PetscReal * wts;
    PetscMalloc(sizeof(PetscReal) * weights.size(), &wts);
    for (std::size_t i = 0; i < weights.size(); ++i)
        wts[i] = weights[i];
    PETSC_CHECK(MatPartitioningSetPartitionWeights(this->obj, wts));
}

void
Partitioning::set_partition_weights(const Real weights[])
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatPartitioningSetPartitionWeights(this->obj, weights));
}

void
Partitioning::view(PetscViewer viewer) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatPartitioningView(this->obj, viewer));
}

} // namespace godzilla
