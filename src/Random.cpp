// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Random.h"
#include "godzilla/CallStack.h"
#include "godzilla/Error.h"

namespace godzilla {

Random::Random() : PetscObjectWrapper(nullptr) {}

void
Random::create(MPI_Comm comm)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscRandomCreate(comm, &this->obj));
}

void
Random::destroy()
{
    CALL_STACK_MSG();
}

void
Random::get_interval(Scalar & low, Scalar & high) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscRandomGetInterval(this->obj, &low, &high));
}

unsigned long
Random::get_seed() const
{
    CALL_STACK_MSG();
    unsigned long seed;
    PETSC_CHECK(PetscRandomGetSeed(this->obj, &seed));
    return seed;
}

std::string
Random::get_type() const
{
    CALL_STACK_MSG();
    PetscRandomType type;
    PETSC_CHECK(PetscRandomGetType(this->obj, &type));
    return std::string(type);
}

Scalar
Random::get_value() const
{
    CALL_STACK_MSG();
    Scalar val;
    PETSC_CHECK(PetscRandomGetValue(this->obj, &val));
    return val;
}

Real
Random::get_value_real() const
{
    CALL_STACK_MSG();
    Real val;
    PETSC_CHECK(PetscRandomGetValueReal(this->obj, &val));
    return val;
}

void
Random::get_values(std::vector<Scalar> & vals) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscRandomGetValues(this->obj, vals.size(), vals.data()));
}

void
Random::get_values_real(std::vector<Real> & vals) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscRandomGetValuesReal(this->obj, vals.size(), vals.data()));
}

void
Random::seed()
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscRandomSeed(this->obj));
}

void
Random::set_interval(Scalar low, Scalar high)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscRandomSetInterval(this->obj, low, high));
}

void
Random::set_seed(unsigned long seed)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscRandomSetSeed(this->obj, seed));
}

void
Random::set_type(const std::string & type)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscRandomSetType(this->obj, type.c_str()));
}

} // namespace godzilla
