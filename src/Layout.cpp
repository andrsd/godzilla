// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Layout.h"

namespace godzilla {

Layout::Layout() : obj(nullptr) {}

Layout::Layout(PetscLayout lo) : obj(lo) {}

void
Layout::create(mpi::Communicator comm)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscLayoutCreate(comm, &this->obj));
}

Layout
Layout::duplicate() const
{
    CALL_STACK_MSG();
    PetscLayout dup;
    PETSC_CHECK(PetscLayoutDuplicate(this->obj, &dup));
    return { dup };
}

int
Layout::find_owner(Int idx) const
{
    CALL_STACK_MSG();
    int rank;
    PETSC_CHECK(PetscLayoutFindOwner(this->obj, idx, &rank));
    return rank;
}

std::tuple<int, Int>
Layout::find_owner_index(Int idx) const
{
    CALL_STACK_MSG();
    int rank;
    Int lidx;
    PETSC_CHECK(PetscLayoutFindOwnerIndex(this->obj, idx, &rank, &lidx));
    return { rank, lidx };
}

Int
Layout::get_block_size() const
{
    CALL_STACK_MSG();
    Int bs;
    PETSC_CHECK(PetscLayoutGetBlockSize(this->obj, &bs));
    return bs;
}

Int
Layout::get_local_size() const
{
    CALL_STACK_MSG();
    Int sz;
    PETSC_CHECK(PetscLayoutGetLocalSize(this->obj, &sz));
    return sz;
}

Range
Layout::get_range() const
{
    CALL_STACK_MSG();
    Int rstart;
    Int rend;
    PETSC_CHECK(PetscLayoutGetRange(this->obj, &rstart, &rend));
    return { rstart, rend };
}

Int
Layout::get_size() const
{
    CALL_STACK_MSG();
    Int n;
    PETSC_CHECK(PetscLayoutGetSize(this->obj, &n));
    return n;
}

void
Layout::set_block_size(Int size)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscLayoutSetBlockSize(this->obj, size));
}

void
Layout::set_local_size(Int size)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscLayoutSetLocalSize(this->obj, size));
}

void
Layout::set_size(Int size)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscLayoutSetSize(this->obj, size));
}

void
Layout::set_up()
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscLayoutSetUp(this->obj));
}

bool
Layout::compare(const Layout & other) const
{
    CALL_STACK_MSG();
    PetscBool congruent;
    PETSC_CHECK(PetscLayoutCompare(this->obj, other.obj, &congruent));
    return congruent == PETSC_TRUE;
}

Layout
Layout::create_from_sizes(mpi::Communicator comm, Int n, Int N, Int bs)
{
    CALL_STACK_MSG();
    PetscLayout layout;
    PETSC_CHECK(PetscLayoutCreateFromSizes(comm, n, N, bs, &layout));
    return { layout };
}

Layout
Layout::create_from_ranges(mpi::Communicator comm, Span<const Int> range, Int bs)
{
    CALL_STACK_MSG();
    expect_true(range.size() == comm.size() + 1,
                fmt::format("Range must have {} entries. Only {} entries supplied.",
                            comm.size() + 1,
                            range.size()));
    PetscLayout layout;
    PETSC_CHECK(PetscLayoutCreateFromRanges(comm, range.data(), PETSC_USE_POINTER, bs, &layout));
    return { layout };
}

} // namespace godzilla
