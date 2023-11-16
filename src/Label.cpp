#include "godzilla/Label.h"
#include "godzilla/CallStack.h"
#include "godzilla/Error.h"

namespace godzilla {

Label::Label() : label(nullptr)
{
    _F_;
}

Label::Label(DMLabel label) : label(label)
{
    _F_;
}

void
Label::create(MPI_Comm comm, const std::string & name)
{
    _F_;
    PETSC_CHECK(DMLabelCreate(comm, name.c_str(), &this->label));
}

void
Label::destroy()
{
    _F_;
    PETSC_CHECK(DMLabelDestroy(&this->label));
    this->label = nullptr;
}

void
Label::reset()
{
    _F_;
    PETSC_CHECK(DMLabelReset(this->label));
}

bool
Label::is_null() const
{
    _F_;
    return this->label == nullptr;
}

void
Label::set_default_value(Int default_value) const
{
    _F_;
    PETSC_CHECK(DMLabelSetDefaultValue(this->label, default_value));
}

Int
Label::get_default_value() const
{
    _F_;
    Int value;
    PETSC_CHECK(DMLabelGetDefaultValue(this->label, &value));
    return value;
}

Int
Label::get_num_values() const
{
    _F_;
    Int n;
    PETSC_CHECK(DMLabelGetNumValues(this->label, &n));
    return n;
}

Int
Label::get_value(Int point) const
{
    _F_;
    Int value;
    PETSC_CHECK(DMLabelGetValue(this->label, point, &value));
    return value;
}

IndexSet
Label::get_value_index_set() const
{
    _F_;
    IS is;
    PETSC_CHECK(DMLabelGetValueIS(this->label, &is));
    return IndexSet(is);
}

std::vector<Int>
Label::get_values() const
{
    _F_;
    auto is = get_value_index_set();
    is.get_indices();
    auto values = is.to_std_vector();
    is.restore_indices();
    is.destroy();
    return values;
}

void
Label::set_value(Int point, Int value)
{
    _F_;
    PETSC_CHECK(DMLabelSetValue(this->label, point, value));
}

Int
Label::get_stratum_size(Int value) const
{
    _F_;
    Int n;
    PETSC_CHECK(DMLabelGetStratumSize(this->label, value, &n));
    return n;
}

IndexSet
Label::get_stratum(Int value) const
{
    _F_;
    IS is;
    PETSC_CHECK(DMLabelGetStratumIS(this->label, value, &is));
    return IndexSet(is);
}

void
Label::set_stratum(Int value, const IndexSet & is) const
{
    _F_;
    PETSC_CHECK(DMLabelSetStratumIS(this->label, value, is));
}

void
Label::view(PetscViewer viewer) const
{
    _F_;
    PETSC_CHECK(DMLabelView(this->label, viewer));
}

Label::operator DMLabel() const
{
    _F_;
    return this->label;
}

} // namespace godzilla
