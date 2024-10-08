// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Label.h"
#include "godzilla/CallStack.h"
#include "godzilla/Error.h"

namespace godzilla {

Label::Label() : label(nullptr)
{
    CALL_STACK_MSG();
}

Label::Label(DMLabel label) : label(label)
{
    CALL_STACK_MSG();
}

void
Label::create(MPI_Comm comm, const std::string & name)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMLabelCreate(comm, name.c_str(), &this->label));
}

void
Label::destroy()
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMLabelDestroy(&this->label));
    this->label = nullptr;
}

void
Label::reset()
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMLabelReset(this->label));
}

bool
Label::is_null() const
{
    CALL_STACK_MSG();
    return this->label == nullptr;
}

void
Label::set_default_value(Int default_value) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMLabelSetDefaultValue(this->label, default_value));
}

Int
Label::get_default_value() const
{
    CALL_STACK_MSG();
    Int value;
    PETSC_CHECK(DMLabelGetDefaultValue(this->label, &value));
    return value;
}

Int
Label::get_num_values() const
{
    CALL_STACK_MSG();
    Int n;
    PETSC_CHECK(DMLabelGetNumValues(this->label, &n));
    return n;
}

Int
Label::get_value(Int point) const
{
    CALL_STACK_MSG();
    Int value;
    PETSC_CHECK(DMLabelGetValue(this->label, point, &value));
    return value;
}

IndexSet
Label::get_value_index_set() const
{
    CALL_STACK_MSG();
    IS is;
    PETSC_CHECK(DMLabelGetValueIS(this->label, &is));
    return IndexSet(is);
}

std::vector<Int>
Label::get_values() const
{
    CALL_STACK_MSG();
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
    CALL_STACK_MSG();
    PETSC_CHECK(DMLabelSetValue(this->label, point, value));
}

Int
Label::get_stratum_size(Int value) const
{
    CALL_STACK_MSG();
    Int n;
    PETSC_CHECK(DMLabelGetStratumSize(this->label, value, &n));
    return n;
}

std::tuple<Int, Int>
Label::get_stratum_bounds(Int value) const
{
    CALL_STACK_MSG();
    Int start, end;
    PETSC_CHECK(DMLabelGetStratumBounds(this->label, value, &start, &end));
    return std::make_tuple(start, end);
}

IndexSet
Label::get_stratum(Int value) const
{
    CALL_STACK_MSG();
    IS is;
    PETSC_CHECK(DMLabelGetStratumIS(this->label, value, &is));
    return IndexSet(is);
}

void
Label::set_stratum(Int value, const IndexSet & is) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMLabelSetStratumIS(this->label, value, is));
}

void
Label::view(PetscViewer viewer) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMLabelView(this->label, viewer));
}

Label::operator DMLabel() const
{
    CALL_STACK_MSG();
    return this->label;
}

Label::operator bool() const
{
    CALL_STACK_MSG();
    return !is_null();
}

} // namespace godzilla
