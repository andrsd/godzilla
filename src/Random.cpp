// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Random.h"
#include "godzilla/CallStack.h"
#include "godzilla/Error.h"

namespace godzilla {

Random::Random() : PetscObjectWrapper(nullptr) {}

void
Random::create(mpi::Communicator comm)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscRandomCreate(comm, &this->obj_));
}

void
Random::get_interval(Scalar & low, Scalar & high) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscRandomGetInterval(this->obj_, &low, &high));
}

unsigned long
Random::get_seed() const
{
    CALL_STACK_MSG();
#if PETSC_VERSION_GE(3, 23, 0)
    PetscInt seed;
    PETSC_CHECK(PetscRandomGetSeed(this->obj_, &seed));
#else
    unsigned long seed;
    PETSC_CHECK(PetscRandomGetSeed(this->obj, &seed));
#endif
    return seed;
}

String
Random::get_type() const
{
    CALL_STACK_MSG();
    PetscRandomType type;
    PETSC_CHECK(PetscRandomGetType(this->obj_, &type));
    return String(type);
}

Scalar
Random::get_value() const
{
    CALL_STACK_MSG();
    Scalar val;
    PETSC_CHECK(PetscRandomGetValue(this->obj_, &val));
    return val;
}

Real
Random::get_value_real() const
{
    CALL_STACK_MSG();
    Real val;
    PETSC_CHECK(PetscRandomGetValueReal(this->obj_, &val));
    return val;
}

void
Random::get_values(std::vector<Scalar> & vals) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscRandomGetValues(this->obj_, vals.size(), vals.data()));
}

void
Random::get_values_real(std::vector<Real> & vals) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscRandomGetValuesReal(this->obj_, vals.size(), vals.data()));
}

void
Random::seed()
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscRandomSeed(this->obj_));
}

void
Random::set_interval(Scalar low, Scalar high)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscRandomSetInterval(this->obj_, low, high));
}

void
Random::set_seed(unsigned long seed)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscRandomSetSeed(this->obj_, seed));
}

void
Random::set_type(String type)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscRandomSetType(this->obj_, type.c_str()));
}

} // namespace godzilla
