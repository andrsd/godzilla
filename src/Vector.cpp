// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Vector.h"
#include "godzilla/Error.h"
#include "godzilla/Exception.h"
#include "godzilla/CallStack.h"
#include <cassert>
#include <petscvec.h>

namespace godzilla {

Vector::Vector() : vec(nullptr) {}

Vector::Vector(MPI_Comm comm) : vec(nullptr)
{
    CALL_STACK_MSG();
    create(comm);
}

Vector::Vector(Vec vec) : vec(vec)
{
    CALL_STACK_MSG();
}

void
Vector::create(MPI_Comm comm)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecCreate(comm, &this->vec));
}

void
Vector::destroy()
{
    CALL_STACK_MSG();
    if (this->vec)
        PETSC_CHECK(VecDestroy(&this->vec));
    this->vec = nullptr;
}

void
Vector::set_up()
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecSetUp(this->vec));
}

void
Vector::set_name(const std::string & name)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscObjectSetName((PetscObject) this->vec, name.c_str()));
}

void
Vector::assemble()
{
    CALL_STACK_MSG();
    assembly_begin();
    assembly_end();
}

void
Vector::assembly_begin()
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecAssemblyBegin(this->vec));
}

void
Vector::assembly_end()
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecAssemblyEnd(this->vec));
}

std::string
Vector::get_type() const
{
    CALL_STACK_MSG();
    VecType type;
    PETSC_CHECK(VecGetType(this->vec, &type));
    return std::string(type);
}

Int
Vector::get_size() const
{
    CALL_STACK_MSG();
    Int sz;
    PETSC_CHECK(VecGetSize(this->vec, &sz));
    return sz;
}

Int
Vector::get_local_size() const
{
    CALL_STACK_MSG();
    Int sz;
    PETSC_CHECK(VecGetLocalSize(this->vec, &sz));
    return sz;
}

void
Vector::get_values(const std::vector<Int> & idx, std::vector<Scalar> & y) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecGetValues(this->vec, (Int) idx.size(), idx.data(), y.data()));
}

void
Vector::abs()
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecAbs(this->vec));
}

Scalar
Vector::dot(const Vector & y) const
{
    CALL_STACK_MSG();
    Scalar val;
    PETSC_CHECK(VecDot(this->vec, y.vec, &val));
    return val;
}

void
Vector::scale(Scalar alpha)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecScale(this->vec, alpha));
}

void
Vector::duplicate(Vector & b) const
{
    CALL_STACK_MSG();
    VecDuplicate(this->vec, &b.vec);
}

Vector
Vector::duplicate() const
{
    Vec dup;
    PETSC_CHECK(VecDuplicate(this->vec, &dup));
    return { dup };
}

void
Vector::copy(Vector & y) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecCopy(this->vec, y.vec));
}

void
Vector::normalize()
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecNormalize(this->vec, nullptr));
}

Scalar
Vector::min() const
{
    CALL_STACK_MSG();
    Scalar val;
    PETSC_CHECK(VecMin(this->vec, nullptr, &val));
    return val;
}

Scalar
Vector::max() const
{
    CALL_STACK_MSG();
    Scalar val;
    PETSC_CHECK(VecMax(this->vec, nullptr, &val));
    return val;
}

void
Vector::chop(Real tol)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecChop(this->vec, tol));
}

#if PETSC_VERSION_GE(3, 20, 0)
void
Vector::filter(Real tol)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecFilter(this->vec, tol));
}
#endif

void
Vector::axpy(Scalar alpha, const Vector & x)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecAXPY(this->vec, alpha, x));
}

void
Vector::axpby(Scalar alpha, Scalar beta, const Vector & x)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecAXPBY(this->vec, alpha, beta, x));
}

void
Vector::aypx(Scalar beta, const Vector & x)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecAYPX(this->vec, beta, x));
}

void
Vector::waxpy(Scalar alpha, const Vector & x, const Vector & y)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecWAXPY(this->vec, alpha, x, y));
}

void
Vector::maxpy(const std::vector<Scalar> & alpha, const std::vector<Vector> & x)
{
    CALL_STACK_MSG();
    if (alpha.size() == x.size()) {
        auto n = alpha.size();
        std::vector<Vec> xx(n);
        for (std::size_t i = 0; i < n; i++)
            xx[i] = (Vec) x[i];
        PETSC_CHECK(VecMAXPY(this->vec, n, alpha.data(), xx.data()));
    }
}

void
Vector::axpbypcz(Scalar alpha, Scalar beta, Scalar gamma, const Vector & x, const Vec & y)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecAXPBYPCZ(this->vec, alpha, beta, gamma, x, y));
}

void
Vector::reciprocal()
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecReciprocal(this->vec));
}

void
Vector::shift(Scalar shift)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecShift(this->vec, shift));
}

void
Vector::set(Scalar alpha)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecSet(this->vec, alpha));
}

void
Vector::set_sizes(Int n, Int N)
{
    CALL_STACK_MSG();
    if (n == PETSC_DECIDE && N == PETSC_DECIDE)
        throw Exception(
            "Calling Vector::set_sizes with n = PETSC_DECIDE and N = PETSC_DECIDE is not allowed.");
    PETSC_CHECK(VecSetSizes(this->vec, n, N));
}

void
Vector::set_block_size(Int bs)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecSetBlockSize(this->vec, bs));
}

void
Vector::set_type(VecType method)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecSetType(this->vec, method));
}

void
Vector::set_value(Int row, Scalar value, InsertMode mode)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecSetValue(this->vec, row, value, mode));
}

void
Vector::set_value_local(Int row, Scalar value, InsertMode mode)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecSetValueLocal(this->vec, row, value, mode));
}

void
Vector::set_values(const std::vector<Int> & ix, const std::vector<Scalar> & y, InsertMode mode)
{
    CALL_STACK_MSG();
    assert(ix.size() == y.size());
    PETSC_CHECK(VecSetValues(this->vec, ix.size(), ix.data(), y.data(), mode));
}

void
Vector::set_values_local(const std::vector<Int> & ix,
                         const std::vector<Scalar> & y,
                         InsertMode mode)
{
    CALL_STACK_MSG();
    assert(ix.size() == y.size());
    PETSC_CHECK(VecSetValuesLocal(this->vec, ix.size(), ix.data(), y.data(), mode));
}

Scalar
Vector::sum() const
{
    CALL_STACK_MSG();
    Scalar sum;
    PETSC_CHECK(VecSum(this->vec, &sum));
    return sum;
}

void
Vector::zero()
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecZeroEntries(this->vec));
}

Scalar
Vector::operator()(Int idx) const
{
    CALL_STACK_MSG();
    Scalar val;
    VecGetValues(this->vec, 1, &idx, &val);
    return val;
}

Scalar *
Vector::get_array()
{
    CALL_STACK_MSG();
    Scalar * array;
    PETSC_CHECK(VecGetArray(this->vec, &array));
    return array;
}

const Scalar *
Vector::get_array_read() const
{
    CALL_STACK_MSG();
    const Scalar * array;
    PETSC_CHECK(VecGetArrayRead(this->vec, &array));
    return array;
}

void
Vector::restore_array(Scalar * array)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecRestoreArray(this->vec, &array));
}

void
Vector::restore_array_read(const Scalar * array) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecRestoreArrayRead(this->vec, &array));
}

void
Vector::view(PetscViewer viewer) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecView(this->vec, viewer));
}

Vector::operator Vec() const
{
    CALL_STACK_MSG();
    return this->vec;
}

Vector
Vector::create_seq(MPI_Comm comm, Int n)
{
    CALL_STACK_MSG();
    Vector v(comm);
    PETSC_CHECK(VecCreateSeq(comm, n, &v.vec));
    return v;
}

Vector
Vector::create_mpi(MPI_Comm comm, Int n, Int N)
{
    CALL_STACK_MSG();
    Vector v(comm);
    PETSC_CHECK(VecCreateMPI(comm, n, N, &v.vec));
    return v;
}

void
Vector::pointwise_min(const Vector & w, const Vector & x, const Vector & y)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecPointwiseMin(w, x, y));
}

void
Vector::pointwise_max(const Vector & w, const Vector & x, const Vector & y)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecPointwiseMax(w, x, y));
}

void
Vector::pointwise_mult(const Vector & w, const Vector & x, const Vector & y)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecPointwiseMult(w, x, y));
}

void
Vector::pointwise_divide(const Vector & w, const Vector & x, const Vector & y)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecPointwiseDivide(w, x, y));
}

void
Vector::set_option(VecOption op, bool flag)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecSetOption(this->vec, op, flag ? PETSC_TRUE : PETSC_FALSE));
}

Vector
Vector::get_sub_vector(const IndexSet & is)
{
    CALL_STACK_MSG();
    Vec y;
    PETSC_CHECK(VecGetSubVector(this->vec, is, &y));
    return Vector(y);
}

void
Vector::restore_sub_vector(const IndexSet & is, Vector & y)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecRestoreSubVector(this->vec, is, &y.vec));
}

} // namespace godzilla
