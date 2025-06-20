// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/IndexSet.h"
#include "godzilla/Error.h"
#include "godzilla/Exception.h"
#include "godzilla/CallStack.h"
#include <cassert>
#include <petscis.h>

namespace godzilla {

IndexSet::Iterator::Iterator(IndexSet * is, Int idx) : is(is), idx(idx) {}

const IndexSet::Iterator::value_type &
IndexSet::Iterator::operator*() const
{
    return this->is->indices[this->idx];
}

IndexSet::Iterator &
IndexSet::Iterator::operator++()
{
    ++this->idx;
    return *this;
}

IndexSet::Iterator
IndexSet::Iterator::operator++(int)
{
    Iterator tmp = *this;
    ++(*this);
    return tmp;
}

bool
operator==(const IndexSet::Iterator & a, const IndexSet::Iterator & b)
{
    return ((IS) a.is == (IS) b.is) && (a.idx == b.idx);
}

bool
operator!=(const IndexSet::Iterator & a, const IndexSet::Iterator & b)
{
    return ((IS) a.is != (IS) b.is) || (a.idx != b.idx);
}

//

IndexSet::IndexSet() : is(nullptr), indices(nullptr) {}

IndexSet::IndexSet(IS is) : is(is), indices(nullptr) {}

const Int *
IndexSet::data() const
{
    CALL_STACK_MSG();
    return this->indices;
}

Int
IndexSet::operator[](Int i) const
{
    CALL_STACK_MSG();
    assert(this->indices != nullptr);
    return this->indices[i];
}

Int
IndexSet::operator()(Int i) const
{
    CALL_STACK_MSG();
    assert(this->indices != nullptr);
    return this->indices[i];
}

void
IndexSet::create(MPI_Comm comm)
{
    CALL_STACK_MSG();
    PETSC_CHECK(ISCreate(comm, &this->is));
}

void
IndexSet::destroy()
{
    CALL_STACK_MSG();
    assert(this->is != nullptr);
    PETSC_CHECK(ISDestroy(&this->is));
    this->is = nullptr;
}

bool
IndexSet::is_null() const
{
    CALL_STACK_MSG();
    return this->is == nullptr;
}

void
IndexSet::restore_indices()
{
    CALL_STACK_MSG();
    if (this->indices != nullptr) {
        assert(this->is != nullptr);
        PETSC_CHECK(ISRestoreIndices(this->is, &this->indices));
        this->indices = nullptr;
    }
}

void
IndexSet::get_point_range(Int & start, Int & end, const Int *& points) const
{
    CALL_STACK_MSG();
    assert(this->is != nullptr);
    PETSC_CHECK(ISGetPointRange(this->is, &start, &end, &points));
}

void
IndexSet::restore_point_range(Int start, Int end, const Int * points) const
{
    CALL_STACK_MSG();
    assert(this->is != nullptr);
    PETSC_CHECK(ISRestorePointRange(this->is, &start, &end, &points));
}

void
IndexSet::get_point_subrange(Int start, Int end, const Int * points) const
{
    CALL_STACK_MSG();
    assert(this->is != nullptr);
    PETSC_CHECK(ISGetPointSubrange(this->is, start, end, points));
}

Int
IndexSet::get_size() const
{
    CALL_STACK_MSG();
    assert(this->is != nullptr);
    Int n;
    PETSC_CHECK(ISGetSize(this->is, &n));
    return n;
}

Int
IndexSet::get_local_size() const
{
    CALL_STACK_MSG();
    assert(this->is != nullptr);
    Int n;
    PETSC_CHECK(ISGetLocalSize(this->is, &n));
    return n;
}

IndexSet
IndexSet::duplicate() const
{
    CALL_STACK_MSG();
    IS new_is;
    PETSC_CHECK(ISDuplicate(this->is, &new_is));
    return IndexSet(new_is);
}

void
IndexSet::get_indices()
{
    CALL_STACK_MSG();
    if (this->is != nullptr)
        PETSC_CHECK(ISGetIndices(this->is, &this->indices));
}

std::vector<Int>
IndexSet::to_std_vector()
{
    CALL_STACK_MSG();
    std::vector<Int> idxs;
    if (this->indices != nullptr) {
        Int n = get_local_size();
        idxs.assign(this->indices, this->indices + n);
    }
    return idxs;
}

IndexSet
IndexSet::create_general(MPI_Comm comm, const std::vector<Int> & idx, CopyMode copy_mode)
{
    CALL_STACK_MSG();
    IS is;
    PETSC_CHECK(ISCreateGeneral(comm, idx.size(), idx.data(), (PetscCopyMode) copy_mode, &is));
    return IndexSet(is);
}

PetscObjectId
IndexSet::get_id() const
{
    CALL_STACK_MSG();
    assert(this->is != nullptr);
    PetscObjectId id;
    PETSC_CHECK(PetscObjectGetId((PetscObject) this->is, &id));
    return id;
}

void
IndexSet::inc_ref()
{
    CALL_STACK_MSG();
    assert(this->is != nullptr);
    PETSC_CHECK(PetscObjectReference((PetscObject) this->is));
}

bool
IndexSet::sorted() const
{
    CALL_STACK_MSG();
    assert(this->is != nullptr);
    PetscBool res;
    PETSC_CHECK(ISSorted(this->is, &res));
    return res == PETSC_TRUE;
}

void
IndexSet::sort() const
{
    CALL_STACK_MSG();
    assert(this->is != nullptr);
    PETSC_CHECK(ISSort(this->is));
}

void
IndexSet::sort_remove_dups() const
{
    CALL_STACK_MSG();
    assert(this->is != nullptr);
    PETSC_CHECK(ISSortRemoveDups(this->is));
}

void
IndexSet::view(PetscViewer viewer) const
{
    CALL_STACK_MSG();
    assert(this->is != nullptr);
    PETSC_CHECK(ISView(this->is, viewer));
}

IndexSet::operator IS() const
{
    CALL_STACK_MSG();
    return this->is;
}

IndexSet::operator IS *()
{
    CALL_STACK_MSG();
    return &(this->is);
}

bool
IndexSet::empty() const
{
    CALL_STACK_MSG();
    assert(this->is != nullptr);
    return get_size() == 0;
}

void
IndexSet::shift(Int offset)
{
    CALL_STACK_MSG();
    PETSC_CHECK(ISShift(this->is, offset, this->is));
}

void
IndexSet::assign(const IndexSet & src)
{
    CALL_STACK_MSG();
    PETSC_CHECK(ISCopy(src, this->is));
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
    IS is;
    ISIntersect(is1, is2, &is);
    return IndexSet(is);
}

void
IndexSet::copy(const IndexSet & src, IndexSet & dest)
{
    CALL_STACK_MSG();
    PETSC_CHECK(ISCopy(src, dest));
}

IndexSet::Iterator
IndexSet::begin()
{
    CALL_STACK_MSG();
    if (this->is == nullptr || this->indices == nullptr)
        return Iterator(this, -1);
    else
        return Iterator(this, 0);
}

IndexSet::ConstIterator
IndexSet::begin() const
{
    if (this->is == nullptr || this->indices == nullptr)
        return ConstIterator(this, -1);
    else
        return ConstIterator(this, 0);
}

IndexSet::Iterator
IndexSet::end()
{
    CALL_STACK_MSG();
    if (this->is == nullptr || this->indices == nullptr)
        return Iterator(this, -1);
    else {
        auto n = get_local_size();
        return Iterator(this, n);
    }
}

IndexSet::ConstIterator
IndexSet::end() const
{
    CALL_STACK_MSG();
    if (this->is == nullptr || this->indices == nullptr)
        return ConstIterator(this, -1);
    else {
        auto n = get_local_size();
        return ConstIterator(this, n);
    }
}

IndexSet
IndexSet::complement(Int nmin, Int nmax) const
{
    CALL_STACK_MSG();
    IS out;
    PETSC_CHECK(ISComplement(this->is, nmin, nmax, &out));
    return IndexSet(out);
}

IndexSet
IndexSet::concatenate(MPI_Comm comm, const std::vector<IndexSet> & is_list)
{
    CALL_STACK_MSG();
    IS out;
    std::vector<IS> is_vec(is_list.size());
    for (size_t i = 0; i < is_list.size(); ++i)
        is_vec[i] = is_list[i];
    PETSC_CHECK(ISConcatenate(comm, is_vec.size(), is_vec.data(), &out));
    return IndexSet(out);
}

IndexSet
IndexSet::difference(const IndexSet & is1, const IndexSet & is2)
{
    CALL_STACK_MSG();
    IS out;
    PETSC_CHECK(ISDifference(is1, is2, &out));
    return IndexSet(out);
}

bool
IndexSet::equal(const IndexSet & other) const
{
    CALL_STACK_MSG();
    PetscBool res;
    PETSC_CHECK(ISEqual(this->is, other, &res));
    return res == PETSC_TRUE;
}

bool
IndexSet::equal_unsorted(const IndexSet & other) const
{
    CALL_STACK_MSG();
    PetscBool res;
    PETSC_CHECK(ISEqualUnsorted(this->is, other, &res));
    return res == PETSC_TRUE;
}

IndexSet
IndexSet::expand(const IndexSet & is1, const IndexSet & is2)
{
    CALL_STACK_MSG();
    IS out;
    PETSC_CHECK(ISExpand(is1, is2, &out));
    return IndexSet(out);
}

std::tuple<Int, Int>
IndexSet::get_min_max() const
{
    CALL_STACK_MSG();
    PetscInt min, max;
    PETSC_CHECK(ISGetMinMax(this->is, &min, &max));
    return std::make_tuple(min, max);
}

std::string
IndexSet::get_type() const
{
    CALL_STACK_MSG();
    ISType type;
    PETSC_CHECK(ISGetType(this->is, &type));
    return std::string(type);
}

void
IndexSet::set_identity()
{
    CALL_STACK_MSG();
    PETSC_CHECK(ISSetIdentity(this->is));
}

bool
IndexSet::identity() const
{
    CALL_STACK_MSG();
    PetscBool res;
    PETSC_CHECK(ISIdentity(this->is, &res));
    return res == PETSC_TRUE;
}

Int
IndexSet::locate(Int key) const
{
    CALL_STACK_MSG();
    PetscInt idx;
    PETSC_CHECK(ISLocate(this->is, key, &idx));
    return idx;
}

void
IndexSet::set_permutation()
{
    CALL_STACK_MSG();
    PETSC_CHECK(ISSetPermutation(this->is));
}

bool
IndexSet::permutation() const
{
    CALL_STACK_MSG();
    PetscBool res;
    PETSC_CHECK(ISPermutation(this->is, &res));
    return res == PETSC_TRUE;
}

void
IndexSet::set_type(const std::string & type)
{
    CALL_STACK_MSG();
    PETSC_CHECK(ISSetType(this->is, type.c_str()));
}

IndexSet
IndexSet::sum(const IndexSet & is1, const IndexSet & is2)
{
    CALL_STACK_MSG();
    IS out;
    PETSC_CHECK(ISSum(is1, is2, &out));
    return IndexSet(out);
}

IndexSet::operator bool() const
{
    return !is_null();
}

IndexSet::operator bool()
{
    return !is_null();
}

} // namespace godzilla
