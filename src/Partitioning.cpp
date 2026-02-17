// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Partitioning.h"
#include "godzilla/CallStack.h"
#include <cstring>
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
Partitioning::set_vertex_weights(Span<Int> weights)
{
    CALL_STACK_MSG();
    // @note This is not a mistake, see doco for `MatPartitioningSetVertexWeights`
    Int * wts;
    PetscMalloc(sizeof(Int) * weights.size(), &wts);
    std::memcpy(wts, std::data(weights), weights.size() * sizeof(Int));
    PETSC_CHECK(MatPartitioningSetVertexWeights(this->obj, wts));
}

void
Partitioning::set_partition_weights(Span<Real> weights)
{
    CALL_STACK_MSG();
    // @note This is not a mistake, see doco
    Real * wts;
    PetscMalloc(sizeof(Real) * weights.size(), &wts);
    std::memcpy(wts, std::data(weights), weights.size() * sizeof(Real));
    PETSC_CHECK(MatPartitioningSetPartitionWeights(this->obj, wts));
}

void
Partitioning::view(PetscViewer viewer) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatPartitioningView(this->obj, viewer));
}

} // namespace godzilla
