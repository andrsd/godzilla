#include "godzilla/Random.h"
#include "godzilla/CallStack.h"
#include "godzilla/Error.h"

namespace godzilla {

Random::Random() : rng(nullptr) {}

void
Random::create(MPI_Comm comm)
{
    _F_;
    PETSC_CHECK(PetscRandomCreate(comm, &this->rng));
}

void
Random::destroy()
{
    _F_;
    PETSC_CHECK(PetscRandomDestroy(&this->rng));
    this->rng = nullptr;
}

void
Random::get_interval(Scalar & low, Scalar & high) const
{
    _F_;
    PETSC_CHECK(PetscRandomGetInterval(this->rng, &low, &high));
}

unsigned long
Random::get_seed() const
{
    _F_;
    unsigned long seed;
    PETSC_CHECK(PetscRandomGetSeed(this->rng, &seed));
    return seed;
}

std::string
Random::get_type() const
{
    _F_;
    PetscRandomType type;
    PETSC_CHECK(PetscRandomGetType(this->rng, &type));
    return std::string(type);
}

Scalar
Random::get_value() const
{
    _F_;
    Scalar val;
    PETSC_CHECK(PetscRandomGetValue(this->rng, &val));
    return val;
}

Real
Random::get_value_real() const
{
    _F_;
    Real val;
    PETSC_CHECK(PetscRandomGetValueReal(this->rng, &val));
    return val;
}

void
Random::get_values(std::vector<Scalar> & vals) const
{
    _F_;
    PETSC_CHECK(PetscRandomGetValues(this->rng, vals.size(), vals.data()));
}

void
Random::get_values_real(std::vector<Real> & vals) const
{
    _F_;
    PETSC_CHECK(PetscRandomGetValuesReal(this->rng, vals.size(), vals.data()));
}

void
Random::seed()
{
    _F_;
    PETSC_CHECK(PetscRandomSeed(this->rng));
}

void
Random::set_interval(Scalar low, Scalar high)
{
    _F_;
    PETSC_CHECK(PetscRandomSetInterval(this->rng, low, high));
}

void
Random::set_seed(unsigned long seed)
{
    _F_;
    PETSC_CHECK(PetscRandomSetSeed(this->rng, seed));
}

void
Random::set_type(const std::string & type)
{
    _F_;
    PETSC_CHECK(PetscRandomSetType(this->rng, type.c_str()));
}

} // namespace godzilla
