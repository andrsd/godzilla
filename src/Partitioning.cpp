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

Partitioning::Partitioning() : matp(nullptr) {}

Partitioning::Partitioning(MatPartitioning mp) : matp(mp) {}

void
Partitioning::create(MPI_Comm comm)
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatPartitioningCreate(comm, &this->matp));
}

void
Partitioning::destroy()
{
    CALL_STACK_MSG();
    // PETSC_CHECK(MatPartitioningDestroy(&this->matp));
    // this->matp = nullptr;
}

std::string
Partitioning::get_type() const
{
    CALL_STACK_MSG();
    MatPartitioningType type;
    PETSC_CHECK(MatPartitioningGetType(this->matp, &type));
    return { type };
}

void
Partitioning::set_type(const std::string & type) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatPartitioningSetType(this->matp, type.c_str()));
}

void
Partitioning::set_n_parts(Int n)
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatPartitioningSetNParts(this->matp, n));
}

bool
Partitioning::get_use_edge_weights() const
{
    CALL_STACK_MSG();
    PetscBool flag;
    PETSC_CHECK(MatPartitioningGetUseEdgeWeights(this->matp, &flag));
    return flag == PETSC_TRUE;
}

void
Partitioning::set_use_edge_weights(bool flag)
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatPartitioningSetUseEdgeWeights(this->matp, flag ? PETSC_TRUE : PETSC_FALSE));
}

IndexSet
Partitioning::apply()
{
    CALL_STACK_MSG();
    IS is;
    PETSC_CHECK(MatPartitioningApply(this->matp, &is));
    return IndexSet(is);
}

void
Partitioning::improve(IndexSet & partitioning)
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatPartitioningImprove(this->matp, partitioning));
}

void
Partitioning::set_adjacency(const Matrix & adj)
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatPartitioningSetAdjacency(this->matp, adj));
}

void
Partitioning::set_number_vertex_weights(Int n)
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatPartitioningSetNumberVertexWeights(this->matp, n));
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
    PETSC_CHECK(MatPartitioningSetVertexWeights(this->matp, wts));
}

void
Partitioning::set_vertex_weights(const Int weights[])
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatPartitioningSetVertexWeights(this->matp, weights));
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
    PETSC_CHECK(MatPartitioningSetPartitionWeights(this->matp, wts));
}

void
Partitioning::set_partition_weights(const Real weights[])
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatPartitioningSetPartitionWeights(this->matp, weights));
}

void
Partitioning::view(PetscViewer viewer) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatPartitioningView(this->matp, viewer));
}

Partitioning::operator MatPartitioning() const
{
    CALL_STACK_MSG();
    return this->matp;
}

Partitioning::operator MatPartitioning()
{
    CALL_STACK_MSG();
    return this->matp;
}

} // namespace godzilla
