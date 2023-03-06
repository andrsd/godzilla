#include "Vector.h"
#include "Error.h"
#include "CallStack.h"
#include <cassert>

namespace godzilla {

Vector::Vector() : vec(nullptr) {}

Vector::Vector(MPI_Comm comm)
{
    _F_;
    create(comm);
}

Vector::Vector(Vec vec) : vec(vec)
{
    _F_;
}

Vector::~Vector() {}

void
Vector::create(MPI_Comm comm)
{
    _F_;
    PETSC_CHECK(VecCreate(comm, &this->vec));
}

void
Vector::destroy()
{
    _F_;
    if (this->vec)
        PETSC_CHECK(VecDestroy(&this->vec));
    this->vec = nullptr;
}

void
Vector::set_name(const std::string & name)
{
    PETSC_CHECK(PetscObjectSetName((PetscObject) this->vec, name.c_str()));
}

void
Vector::assembly_begin()
{
    _F_;
    PETSC_CHECK(VecAssemblyBegin(this->vec));
}

void
Vector::assembly_end()
{
    _F_;
    PETSC_CHECK(VecAssemblyEnd(this->vec));
}

Int
Vector::get_size() const
{
    _F_;
    Int sz;
    PETSC_CHECK(VecGetSize(this->vec, &sz));
    return sz;
}

Int
Vector::get_local_size() const
{
    _F_;
    Int sz;
    PETSC_CHECK(VecGetLocalSize(this->vec, &sz));
    return sz;
}

void
Vector::get_values(const std::vector<Int> & idx, std::vector<Scalar> & y) const
{
    _F_;
    PETSC_CHECK(VecGetValues(this->vec, (Int) idx.size(), idx.data(), y.data()));
}

void
Vector::abs()
{
    _F_;
    PETSC_CHECK(VecAbs(this->vec));
}

Scalar
Vector::dot(const Vector & y) const
{
    _F_;
    Scalar val;
    PETSC_CHECK(VecDot(this->vec, y.vec, &val));
    return val;
}

void
Vector::scale(Scalar alpha)
{
    _F_;
    PETSC_CHECK(VecScale(this->vec, alpha));
}

void
Vector::duplicate(Vector & b) const
{
    _F_;
    VecDuplicate(this->vec, &b.vec);
}

void
Vector::copy(Vector & y)
{
    _F_;
    PETSC_CHECK(VecCopy(this->vec, y.vec));
}

void
Vector::normalize()
{
    _F_;
    PETSC_CHECK(VecNormalize(this->vec, nullptr));
}

Scalar
Vector::min() const
{
    _F_;
    Scalar val;
    PETSC_CHECK(VecMin(this->vec, nullptr, &val));
    return val;
}

Scalar
Vector::max() const
{
    _F_;
    Scalar val;
    PETSC_CHECK(VecMax(this->vec, nullptr, &val));
    return val;
}

void
Vector::chop(Real tol)
{
    _F_;
    PETSC_CHECK(VecChop(this->vec, tol));
}

void
Vector::axpy(Scalar alpha, const Vector & x)
{
    _F_;
    PETSC_CHECK(VecAXPY(this->vec, alpha, x));
}

void
Vector::aypx(Scalar beta, const Vector & x)
{
    _F_;
    PETSC_CHECK(VecAYPX(this->vec, beta, x));
}

void
Vector::shift(Scalar shift)
{
    _F_;
    PETSC_CHECK(VecShift(this->vec, shift));
}

void
Vector::set(Scalar alpha)
{
    _F_;
    PETSC_CHECK(VecSet(this->vec, alpha));
}

void
Vector::set_sizes(Int n, Int N)
{
    _F_;
    if (n == PETSC_DECIDE && N == PETSC_DECIDE)
        error(
            "Calling Vector::set_sizes with n = PETSC_DECIDE and N = PETSC_DECIDE is not allowed.");
    PETSC_CHECK(VecSetSizes(this->vec, n, N));
}

void
Vector::set_type(VecType method)
{
    _F_;
    PETSC_CHECK(VecSetType(this->vec, method));
}

void
Vector::set_value(Int row, Scalar value, InsertMode mode)
{
    _F_;
    PETSC_CHECK(VecSetValue(this->vec, row, value, mode));
}

void
Vector::set_values(const std::vector<Int> & ix, const std::vector<Scalar> & y, InsertMode mode)
{
    _F_;
    assert(ix.size() == y.size());
    PETSC_CHECK(VecSetValues(this->vec, ix.size(), ix.data(), y.data(), mode));
}

void
Vector::set_values_local(const std::vector<Int> & ix,
                         const std::vector<Scalar> & y,
                         InsertMode mode)
{
    _F_;
    assert(ix.size() == y.size());
    PETSC_CHECK(VecSetValuesLocal(this->vec, ix.size(), ix.data(), y.data(), mode));
}

Scalar
Vector::sum() const
{
    _F_;
    Scalar sum;
    PETSC_CHECK(VecSum(this->vec, &sum));
    return sum;
}

void
Vector::zero()
{
    _F_;
    PETSC_CHECK(VecZeroEntries(this->vec));
}

Scalar
Vector::operator()(Int idx) const
{
    _F_;
    Scalar val;
    VecGetValues(this->vec, 1, &idx, &val);
    return val;
}

Scalar *
Vector::get_array()
{
    _F_;
    Scalar * array;
    PETSC_CHECK(VecGetArray(this->vec, &array));
    return array;
}

void
Vector::restore_array(Scalar * array)
{
    _F_;
    PETSC_CHECK(VecRestoreArray(this->vec, &array));
}

void
Vector::view(PetscViewer viewer) const
{
    _F_;
    PETSC_CHECK(VecView(this->vec, viewer));
}

Vector::operator Vec() const
{
    _F_;
    return this->vec;
}

Vector
Vector::create_seq(MPI_Comm comm, Int n)
{
    _F_;
    Vector v(comm);
    PETSC_CHECK(VecCreateSeq(comm, n, &v.vec));
    return v;
}

void
Vector::pointwise_min(const Vector & w, const Vector & x, const Vector & y)
{
    _F_;
    PETSC_CHECK(VecPointwiseMin(w, x, y));
}

void
Vector::pointwise_max(const Vector & w, const Vector & x, const Vector & y)
{
    _F_;
    PETSC_CHECK(VecPointwiseMax(w, x, y));
}

void
Vector::pointwise_mult(const Vector & w, const Vector & x, const Vector & y)
{
    _F_;
    PETSC_CHECK(VecPointwiseMult(w, x, y));
}

void
Vector::pointwise_divide(const Vector & w, const Vector & x, const Vector & y)
{
    _F_;
    PETSC_CHECK(VecPointwiseDivide(w, x, y));
}

} // namespace godzilla
