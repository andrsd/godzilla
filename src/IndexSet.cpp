#include "IndexSet.h"
#include "Error.h"
#include "CallStack.h"
#include <cassert>

namespace godzilla {

IndexSet::IndexSet() : is(nullptr), indices(nullptr) {}

IndexSet::IndexSet(IS is) : is(is), indices(nullptr) {}

IndexSet::~IndexSet() {}

const Int *
IndexSet::data() const
{
    _F_;
    return this->indices;
}

Int
IndexSet::operator[](unsigned int i) const
{
    _F_;
    return this->indices[i];
}

Int
IndexSet::operator()(unsigned int i) const
{
    _F_;
    return this->indices[i];
}

void
IndexSet::create(MPI_Comm comm)
{
    _F_;
    PETSC_CHECK(ISCreate(comm, &this->is));
}

void
IndexSet::destroy()
{
    _F_;
    PETSC_CHECK(ISDestroy(&this->is));
}

void
IndexSet::restore_indices()
{
    _F_;
    assert(this->is != nullptr);
    PETSC_CHECK(ISRestoreIndices(this->is, &this->indices));
}

Int
IndexSet::get_size() const
{
    _F_;
    assert(this->is != nullptr);
    Int n;
    PETSC_CHECK(ISGetSize(this->is, &n));
    return n;
}

Int
IndexSet::get_local_size() const
{
    _F_;
    assert(this->is != nullptr);
    Int n;
    PETSC_CHECK(ISGetLocalSize(this->is, &n));
    return n;
}

void
IndexSet::get_indices()
{
    _F_;
    assert(this->is != nullptr);
    PETSC_CHECK(ISGetIndices(this->is, &this->indices));
}

std::vector<Int>
IndexSet::to_std_vector()
{
    _F_;
    std::vector<Int> idxs;
    Int n = get_size();
    idxs.assign(this->indices, this->indices + n);
    return idxs;
}

IndexSet
IndexSet::create_general(MPI_Comm comm, const std::vector<Int> & idx)
{
    _F_;
    IS is;
    PETSC_CHECK(ISCreateGeneral(comm, idx.size(), idx.data(), PETSC_COPY_VALUES, &is));
    return IndexSet(is);
}

IndexSet
IndexSet::stratum_from_label(DMLabel label, Int stratum_value)
{
    _F_;
    IS is;
    PETSC_CHECK(DMLabelGetStratumIS(label, stratum_value, &is));
    return IndexSet(is);
}

PetscObjectId
IndexSet::get_id() const
{
    _F_;
    PetscObjectId id;
    PETSC_CHECK(PetscObjectGetId((PetscObject) this->is, &id));
    return id;
}

void
IndexSet::inc_ref()
{
    _F_;
    PETSC_CHECK(PetscObjectReference((PetscObject) this->is));
}

bool
IndexSet::sorted() const
{
    _F_;
    PetscBool res;
    PETSC_CHECK(ISSorted(this->is, &res));
    return res == PETSC_TRUE;
}

void
IndexSet::sort() const
{
    _F_;
    PETSC_CHECK(ISSort(this->is));
}

void
IndexSet::sort_remove_dups() const
{
    _F_;
    PETSC_CHECK(ISSortRemoveDups(this->is));
}

IndexSet::operator IS() const
{
    _F_;
    return this->is;
}

IndexSet::operator IS *()
{
    _F_;
    return &(this->is);
}

bool
IndexSet::empty() const
{
    _F_;
    return this->is == nullptr;
}

IndexSet
IndexSet::intersect_caching(const IndexSet & is1, const IndexSet & is2)
{
    _F_;
    if (!is2.empty() && !is1.empty()) {
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
    _F_;
    IS is;
    ISIntersect(is1, is2, &is);
    return IndexSet(is);
}

} // namespace godzilla
