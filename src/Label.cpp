// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Label.h"
#include "godzilla/CallStack.h"
#include "godzilla/Error.h"

namespace godzilla {

Label::Label() : PetscObjectWrapper(nullptr)
{
    CALL_STACK_MSG();
}

Label::Label(DMLabel label) : PetscObjectWrapper(label)
{
    CALL_STACK_MSG();
}

void
Label::create(MPI_Comm comm, const std::string & name)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMLabelCreate(comm, name.c_str(), &this->obj));
}

void
Label::destroy()
{
    CALL_STACK_MSG();
}

void
Label::reset()
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMLabelReset(this->obj));
}

void
Label::set_default_value(Int default_value) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMLabelSetDefaultValue(this->obj, default_value));
}

Int
Label::get_default_value() const
{
    CALL_STACK_MSG();
    Int value;
    PETSC_CHECK(DMLabelGetDefaultValue(this->obj, &value));
    return value;
}

Int
Label::get_num_values() const
{
    CALL_STACK_MSG();
    Int n;
    PETSC_CHECK(DMLabelGetNumValues(this->obj, &n));
    return n;
}

Int
Label::get_value(Int point) const
{
    CALL_STACK_MSG();
    Int value;
    PETSC_CHECK(DMLabelGetValue(this->obj, point, &value));
    return value;
}

IndexSet
Label::get_value_index_set() const
{
    CALL_STACK_MSG();
    IndexSet is;
    PETSC_CHECK(DMLabelGetValueIS(this->obj, is));
    return is;
}

std::vector<Int>
Label::get_values() const
{
    CALL_STACK_MSG();
    auto is = get_value_index_set();
    is.get_indices();
    auto values = is.to_std_vector();
    is.restore_indices();
    return values;
}

void
Label::set_value(Int point, Int value)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMLabelSetValue(this->obj, point, value));
}

Int
Label::get_stratum_size(Int value) const
{
    CALL_STACK_MSG();
    Int n;
    PETSC_CHECK(DMLabelGetStratumSize(this->obj, value, &n));
    return n;
}

std::tuple<Int, Int>
Label::get_stratum_bounds(Int value) const
{
    CALL_STACK_MSG();
    Int start, end;
    PETSC_CHECK(DMLabelGetStratumBounds(this->obj, value, &start, &end));
    return std::make_tuple(start, end);
}

IndexSet
Label::get_stratum(Int value) const
{
    CALL_STACK_MSG();
    IndexSet is;
    PETSC_CHECK(DMLabelGetStratumIS(this->obj, value, is));
    return is;
}

void
Label::set_stratum(Int value, const IndexSet & is) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMLabelSetStratumIS(this->obj, value, is));
}

void
Label::view(PetscViewer viewer) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMLabelView(this->obj, viewer));
}

} // namespace godzilla
