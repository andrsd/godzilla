#include "IndexSet.h"
#include "Error.h"
#include <cassert>

namespace godzilla {

IndexSet::IndexSet() : is(nullptr), n(0), indices(nullptr) {}

IndexSet::IndexSet(IS is) : is(is), n(0), indices(nullptr) {}

IndexSet::~IndexSet()
{
    if (this->indices)
        PETSC_CHECK(ISRestoreIndices(is, &indices));
    if (this->is)
        PETSC_CHECK(ISDestroy(&this->is));
}

PetscInt
IndexSet::size() const
{
    return this->n;
}

const PetscInt *
IndexSet::data() const
{
    return this->indices;
}

PetscInt
IndexSet::operator[](unsigned int i) const
{
    assert(i < this->n);
    return this->indices[i];
}

void
IndexSet::create(MPI_Comm comm)
{
    PETSC_CHECK(ISCreate(comm, &this->is));
}

void
IndexSet::get_indices_internal()
{
    PETSC_CHECK(ISGetSize(this->is, &this->n));
    PETSC_CHECK(ISGetIndices(this->is, &this->indices));
}

std::vector<PetscInt>
IndexSet::to_std_vector()
{
    std::vector<PetscInt> idxs;
    idxs.assign(this->indices, this->indices + this->n);
    return idxs;
}

IndexSet
IndexSet::values_from_label(DMLabel label)
{
    IS is;
    PETSC_CHECK(DMLabelGetValueIS(label, &is));
    IndexSet obj(is);
    obj.get_indices_internal();
    return obj;
}

IndexSet
IndexSet::stratum_from_label(DMLabel label, PetscInt stratum_value)
{
    IS is;
    PETSC_CHECK(DMLabelGetStratumIS(label, stratum_value, &is));
    IndexSet obj(is);
    obj.get_indices_internal();
    return obj;
}

} // namespace godzilla
