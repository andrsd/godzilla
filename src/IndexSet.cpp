#include "IndexSet.h"
#include "Error.h"
#include <cassert>

namespace godzilla {

IndexSet::IndexSet() : is(nullptr), indices(nullptr) {}

IndexSet::IndexSet(IS is) : is(is), indices(nullptr) {}

IndexSet::~IndexSet() {}

const Int *
IndexSet::data() const
{
    return this->indices;
}

Int
IndexSet::operator[](unsigned int i) const
{
    return this->indices[i];
}

Int
IndexSet::operator()(unsigned int i) const
{
    return this->indices[i];
}

void
IndexSet::create(MPI_Comm comm)
{
    PETSC_CHECK(ISCreate(comm, &this->is));
}

void
IndexSet::destroy()
{
    PETSC_CHECK(ISDestroy(&this->is));
}

void
IndexSet::restore_indices()
{
    assert(this->is != nullptr);
    PETSC_CHECK(ISRestoreIndices(this->is, &this->indices));
}

Int
IndexSet::get_size() const
{
    assert(this->is != nullptr);
    Int n;
    PETSC_CHECK(ISGetSize(this->is, &n));
    return n;
}

Int
IndexSet::get_local_size() const
{
    assert(this->is != nullptr);
    Int n;
    PETSC_CHECK(ISGetLocalSize(this->is, &n));
    return n;
}

void
IndexSet::get_indices()
{
    assert(this->is != nullptr);
    PETSC_CHECK(ISGetIndices(this->is, &this->indices));
}

std::vector<Int>
IndexSet::to_std_vector()
{
    std::vector<Int> idxs;
    Int n = get_size();
    idxs.assign(this->indices, this->indices + n);
    return idxs;
}

IndexSet
IndexSet::values_from_label(DMLabel label)
{
    IS is;
    PETSC_CHECK(DMLabelGetValueIS(label, &is));
    return IndexSet(is);
}

IndexSet
IndexSet::stratum_from_label(DMLabel label, Int stratum_value)
{
    IS is;
    PETSC_CHECK(DMLabelGetStratumIS(label, stratum_value, &is));
    return IndexSet(is);
}

PetscObjectId
IndexSet::get_id() const
{
    PetscObjectId id;
    PETSC_CHECK(PetscObjectGetId((PetscObject) this->is, &id));
    return id;
}

void
IndexSet::inc_ref()
{
    PETSC_CHECK(PetscObjectReference((PetscObject) this->is));
}

IndexSet::operator IS() const
{
    return this->is;
}

bool
IndexSet::empty() const
{
    return this->is == nullptr;
}

IndexSet
IndexSet::intersect_caching(const IndexSet & is1, const IndexSet & is2)
{
    if (!is2.empty() && !is1.empty()) {
        PetscObjectId is2id = is2.get_id();
        char compose_str[33] = { 0 };
        PETSC_CHECK(PetscSNPrintf(compose_str, 32, "ISIntersect_Caching_%" PetscInt64_FMT, is2id));
        IS isect;
        PETSC_CHECK(PetscObjectQuery((PetscObject) (IS) is1, compose_str, (PetscObject *) &isect));
        if (isect == nullptr) {
            PETSC_CHECK(ISIntersect((IS) is1, (IS) is2, &isect));
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

} // namespace godzilla
