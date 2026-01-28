// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/IndexSet.h"
#include "godzilla/Error.h"
#include "godzilla/Exception.h"
#include "godzilla/CallStack.h"
#include "godzilla/Assert.h"
#include <petscis.h>

namespace godzilla {

IndexSet::IndexSet() : PetscObjectWrapper(nullptr) {}

IndexSet::IndexSet(IS is) : PetscObjectWrapper(is) {}

void
IndexSet::create(MPI_Comm comm)
{
    CALL_STACK_MSG();
    PETSC_CHECK(ISCreate(comm, &this->obj));
}

void
IndexSet::destroy()
{
    CALL_STACK_MSG();
}

bool
IndexSet::is_null() const
{
    CALL_STACK_MSG();
    return this->obj == nullptr;
}

void
IndexSet::get_point_range(Int & start, Int & end, const Int *& points) const
{
    CALL_STACK_MSG();
    GODZILLA_ASSERT_TRUE(this->obj != nullptr, "IndexSet is null");
    PETSC_CHECK(ISGetPointRange(this->obj, &start, &end, &points));
}

void
IndexSet::restore_point_range(Int start, Int end, const Int * points) const
{
    CALL_STACK_MSG();
    GODZILLA_ASSERT_TRUE(this->obj != nullptr, "IndexSet is null");
    PETSC_CHECK(ISRestorePointRange(this->obj, &start, &end, &points));
}

void
IndexSet::get_point_subrange(Int start, Int end, const Int * points) const
{
    CALL_STACK_MSG();
    GODZILLA_ASSERT_TRUE(this->obj != nullptr, "IndexSet is null");
    PETSC_CHECK(ISGetPointSubrange(this->obj, start, end, points));
}

Int
IndexSet::get_size() const
{
    CALL_STACK_MSG();
    GODZILLA_ASSERT_TRUE(this->obj != nullptr, "IndexSet is null");
    Int n;
    PETSC_CHECK(ISGetSize(this->obj, &n));
    return n;
}

Int
IndexSet::get_local_size() const
{
    CALL_STACK_MSG();
    GODZILLA_ASSERT_TRUE(this->obj != nullptr, "IndexSet is null");
    Int n;
    PETSC_CHECK(ISGetLocalSize(this->obj, &n));
    return n;
}

IndexSet
IndexSet::duplicate() const
{
    CALL_STACK_MSG();
    IndexSet new_is;
    PETSC_CHECK(ISDuplicate(this->obj, new_is));
    return new_is;
}

IndexSet
IndexSet::create_general(MPI_Comm comm, const std::vector<Int> & idx, CopyMode copy_mode)
{
    CALL_STACK_MSG();
    IndexSet is;
    PETSC_CHECK(ISCreateGeneral(comm, idx.size(), idx.data(), (PetscCopyMode) copy_mode, is));
    return is;
}

bool
IndexSet::sorted() const
{
    CALL_STACK_MSG();
    GODZILLA_ASSERT_TRUE(this->obj != nullptr, "IndexSet is null");
    PetscBool res;
    PETSC_CHECK(ISSorted(this->obj, &res));
    return res == PETSC_TRUE;
}

void
IndexSet::sort() const
{
    CALL_STACK_MSG();
    GODZILLA_ASSERT_TRUE(this->obj != nullptr, "IndexSet is null");
    PETSC_CHECK(ISSort(this->obj));
}

void
IndexSet::sort_remove_dups() const
{
    CALL_STACK_MSG();
    GODZILLA_ASSERT_TRUE(this->obj != nullptr, "IndexSet is null");
    PETSC_CHECK(ISSortRemoveDups(this->obj));
}

void
IndexSet::view(PetscViewer viewer) const
{
    CALL_STACK_MSG();
    GODZILLA_ASSERT_TRUE(this->obj != nullptr, "IndexSet is null");
    PETSC_CHECK(ISView(this->obj, viewer));
}

bool
IndexSet::empty() const
{
    CALL_STACK_MSG();
    GODZILLA_ASSERT_TRUE(this->obj != nullptr, "IndexSet is null");
    return get_size() == 0;
}

void
IndexSet::shift(Int offset)
{
    CALL_STACK_MSG();
    PETSC_CHECK(ISShift(this->obj, offset, this->obj));
}

void
IndexSet::assign(const IndexSet & src)
{
    CALL_STACK_MSG();
    PETSC_CHECK(ISCopy(src, this->obj));
}

IndexSet
IndexSet::intersect_caching(const IndexSet & is1, const IndexSet & is2)
{
    CALL_STACK_MSG();
    if (is2 && is1) {
        PetscObjectId is2id = is2.get_id();
        char compose_str[33] = { 0 };
        PETSC_CHECK(PetscSNPrintf(compose_str, 32, "ISIntersect_Caching_%" PetscInt64_FMT, is2id));
        IS isect;
        PETSC_CHECK(PetscObjectQuery((PetscObject) (IS) is1, compose_str, (PetscObject *) &isect));
        if (isect == nullptr) {
            PETSC_CHECK(ISIntersect(is1, is2, &isect));
            PETSC_CHECK(
                PetscObjectCompose((PetscObject) (IS) is1, compose_str, (PetscObject) isect));
        }
        else {
            PETSC_CHECK(PetscObjectReference((PetscObject) isect));
        }
        return IndexSet(isect);
    }
    else
        return IndexSet();
}

IndexSet
IndexSet::intersect(const IndexSet & is1, const IndexSet & is2)
{
    CALL_STACK_MSG();
    IndexSet is;
    PETSC_CHECK(ISIntersect(is1, is2, is));
    return is;
}

void
IndexSet::copy(const IndexSet & src, IndexSet & dest)
{
    CALL_STACK_MSG();
    PETSC_CHECK(ISCopy(src, dest));
}

IndexSet
IndexSet::complement(Int nmin, Int nmax) const
{
    CALL_STACK_MSG();
    IndexSet out;
    PETSC_CHECK(ISComplement(this->obj, nmin, nmax, out));
    return out;
}

IndexSet
IndexSet::concatenate(MPI_Comm comm, const std::vector<IndexSet> & is_list)
{
    CALL_STACK_MSG();
    IndexSet out;
    std::vector<IS> is_vec(is_list.size());
    for (size_t i = 0; i < is_list.size(); ++i)
        is_vec[i] = is_list[i];
    PETSC_CHECK(ISConcatenate(comm, is_vec.size(), is_vec.data(), out));
    return out;
}

IndexSet
IndexSet::difference(const IndexSet & is1, const IndexSet & is2)
{
    CALL_STACK_MSG();
    IndexSet out;
    PETSC_CHECK(ISDifference(is1, is2, out));
    return out;
}

bool
IndexSet::equal(const IndexSet & other) const
{
    CALL_STACK_MSG();
    PetscBool res;
    PETSC_CHECK(ISEqual(this->obj, other, &res));
    return res == PETSC_TRUE;
}

bool
IndexSet::equal_unsorted(const IndexSet & other) const
{
    CALL_STACK_MSG();
    PetscBool res;
    PETSC_CHECK(ISEqualUnsorted(this->obj, other, &res));
    return res == PETSC_TRUE;
}

IndexSet
IndexSet::expand(const IndexSet & is1, const IndexSet & is2)
{
    CALL_STACK_MSG();
    IndexSet out;
    PETSC_CHECK(ISExpand(is1, is2, out));
    return out;
}

std::tuple<Int, Int>
IndexSet::get_min_max() const
{
    CALL_STACK_MSG();
    PetscInt min, max;
    PETSC_CHECK(ISGetMinMax(this->obj, &min, &max));
    return std::make_tuple(min, max);
}

IndexSetBorrowedIndices
IndexSet::borrow_indices()
{
    CALL_STACK_MSG();
    if (this->obj)
        return IndexSetBorrowedIndices(*this);
    else
        return IndexSetBorrowedIndices();
}

String
IndexSet::get_type() const
{
    CALL_STACK_MSG();
    ISType type;
    PETSC_CHECK(ISGetType(this->obj, &type));
    return String(type);
}

void
IndexSet::set_identity()
{
    CALL_STACK_MSG();
    PETSC_CHECK(ISSetIdentity(this->obj));
}

bool
IndexSet::identity() const
{
    CALL_STACK_MSG();
    PetscBool res;
    PETSC_CHECK(ISIdentity(this->obj, &res));
    return res == PETSC_TRUE;
}

Int
IndexSet::locate(Int key) const
{
    CALL_STACK_MSG();
    PetscInt idx;
    PETSC_CHECK(ISLocate(this->obj, key, &idx));
    return idx;
}

void
IndexSet::set_permutation()
{
    CALL_STACK_MSG();
    PETSC_CHECK(ISSetPermutation(this->obj));
}

bool
IndexSet::permutation() const
{
    CALL_STACK_MSG();
    PetscBool res;
    PETSC_CHECK(ISPermutation(this->obj, &res));
    return res == PETSC_TRUE;
}

void
IndexSet::set_type(String type)
{
    CALL_STACK_MSG();
    PETSC_CHECK(ISSetType(this->obj, type.c_str()));
}

IndexSet
IndexSet::sum(const IndexSet & is1, const IndexSet & is2)
{
    CALL_STACK_MSG();
    IndexSet out;
    PETSC_CHECK(ISSum(is1, is2, out));
    return out;
}

//

IndexSetBorrowedIndices::ConstIterator
IndexSetBorrowedIndices::begin() const
{
    if (this->data_ == nullptr)
        return ConstIterator(nullptr, -1);
    else
        return ConstIterator(this->data_, 0);
}

IndexSetBorrowedIndices::ConstIterator
IndexSetBorrowedIndices::end() const
{
    CALL_STACK_MSG();
    if (this->data_ == nullptr)
        return ConstIterator(this->data_, -1);
    else
        return ConstIterator(this->data_, this->size_);
}

} // namespace godzilla
