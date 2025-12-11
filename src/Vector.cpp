// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Vector.h"
#include "godzilla/Error.h"
#include "godzilla/Exception.h"
#include "godzilla/CallStack.h"
#include "godzilla/NestVector.h"
#include <petscvec.h>

namespace godzilla {

Vector::Vector() : PetscObjectWrapper(nullptr) {}

Vector::Vector(MPI_Comm comm) : PetscObjectWrapper(nullptr)
{
    CALL_STACK_MSG();
    create(comm);
}

Vector::Vector(Vec vec) : PetscObjectWrapper(vec)
{
    CALL_STACK_MSG();
}

void
Vector::create(MPI_Comm comm)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecCreate(comm, &this->obj));
}

void
Vector::destroy()
{
    CALL_STACK_MSG();
}

void
Vector::set_up()
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecSetUp(this->obj));
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
    PETSC_CHECK(VecAssemblyBegin(this->obj));
}

void
Vector::assembly_end()
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecAssemblyEnd(this->obj));
}

String
Vector::get_type() const
{
    CALL_STACK_MSG();
    VecType type;
    PETSC_CHECK(VecGetType(this->obj, &type));
    return String(type);
}

Int
Vector::get_size() const
{
    CALL_STACK_MSG();
    Int sz;
    PETSC_CHECK(VecGetSize(this->obj, &sz));
    return sz;
}

Int
Vector::get_local_size() const
{
    CALL_STACK_MSG();
    Int sz;
    PETSC_CHECK(VecGetLocalSize(this->obj, &sz));
    return sz;
}

void
Vector::get_values(const std::vector<Int> & idx, std::vector<Scalar> & y) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecGetValues(this->obj, (Int) idx.size(), idx.data(), y.data()));
}

void
Vector::abs()
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecAbs(this->obj));
}

void
Vector::scale(Scalar alpha)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecScale(this->obj, alpha));
}

Vector
Vector::duplicate() const
{
    Vector dup;
    PETSC_CHECK(VecDuplicate(this->obj, dup));
    return dup;
}

void
Vector::assign(const Vector & y)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecCopy(y.obj, this->obj));
}

void
Vector::normalize()
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecNormalize(this->obj, nullptr));
}

Scalar
Vector::min() const
{
    CALL_STACK_MSG();
    Scalar val;
    PETSC_CHECK(VecMin(this->obj, nullptr, &val));
    return val;
}

Scalar
Vector::max() const
{
    CALL_STACK_MSG();
    Scalar val;
    PETSC_CHECK(VecMax(this->obj, nullptr, &val));
    return val;
}

void
Vector::chop(Real tol)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecFilter(this->obj, tol));
}

#if PETSC_VERSION_GE(3, 20, 0)
void
Vector::filter(Real tol)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecFilter(this->obj, tol));
}
#endif

void
Vector::reciprocal()
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecReciprocal(this->obj));
}

void
Vector::shift(Scalar shift)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecShift(this->obj, shift));
}

void
Vector::set(Scalar alpha)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecSet(this->obj, alpha));
}

void
Vector::set_sizes(Int n, Int N)
{
    CALL_STACK_MSG();
    if (n == PETSC_DECIDE && N == PETSC_DECIDE)
        throw Exception(
            "Calling Vector::set_sizes with n = PETSC_DECIDE and N = PETSC_DECIDE is not allowed.");
    PETSC_CHECK(VecSetSizes(this->obj, n, N));
}

void
Vector::set_block_size(Int bs)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecSetBlockSize(this->obj, bs));
}

void
Vector::set_type(VecType method)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecSetType(this->obj, method));
}

void
Vector::set_value(Int row, Scalar value, InsertMode mode)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecSetValue(this->obj, row, value, mode));
}

void
Vector::set_value_local(Int row, Scalar value, InsertMode mode)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecSetValueLocal(this->obj, row, value, mode));
}

void
Vector::set_values(Int n, const Int * ix, const Scalar * y, InsertMode mode)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecSetValues(this->obj, n, ix, y, mode));
}

void
Vector::set_values(const std::vector<Int> & ix, const std::vector<Scalar> & y, InsertMode mode)
{
    CALL_STACK_MSG();
    if (ix.size() == y.size())
        PETSC_CHECK(VecSetValues(this->obj, ix.size(), ix.data(), y.data(), mode));
    else
        throw Exception("Number of indices does not match the number of values");
}

void
Vector::set_values(const DynDenseVector<Int> & ix,
                   const DynDenseVector<Scalar> & y,
                   InsertMode mode)
{
    CALL_STACK_MSG();
    if (ix.size() == y.size())
        PETSC_CHECK(VecSetValues(this->obj, ix.size(), ix.data(), y.data(), mode));
    else
        throw Exception("Number of indices does not match the number of values");
}

void
Vector::set_values_local(const std::vector<Int> & ix,
                         const std::vector<Scalar> & y,
                         InsertMode mode)
{
    CALL_STACK_MSG();
    if (ix.size() == y.size())
        PETSC_CHECK(VecSetValuesLocal(this->obj, ix.size(), ix.data(), y.data(), mode));
    else
        throw Exception("Number of indices does not match the number of values");
}

void
Vector::set_values_local(const DynDenseVector<Int> & ix,
                         const DynDenseVector<Scalar> & y,
                         InsertMode mode)
{
    CALL_STACK_MSG();
    if (ix.size() == y.size())
        PETSC_CHECK(VecSetValuesLocal(this->obj, ix.size(), ix.data(), y.data(), mode));
    else
        throw Exception("Number of indices does not match the number of values");
}

Scalar
Vector::sum() const
{
    CALL_STACK_MSG();
    Scalar sum;
    PETSC_CHECK(VecSum(this->obj, &sum));
    return sum;
}

void
Vector::zero()
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecZeroEntries(this->obj));
}

Scalar
Vector::operator()(Int idx) const
{
    CALL_STACK_MSG();
    Scalar val;
    PETSC_CHECK(VecGetValues(this->obj, 1, &idx, &val));
    return val;
}

Scalar *
Vector::get_array()
{
    CALL_STACK_MSG();
    Scalar * array;
    PETSC_CHECK(VecGetArray(this->obj, &array));
    return array;
}

const Scalar *
Vector::get_array_read() const
{
    CALL_STACK_MSG();
    const Scalar * array;
    PETSC_CHECK(VecGetArrayRead(this->obj, &array));
    return array;
}

void
Vector::restore_array(Scalar * array)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecRestoreArray(this->obj, &array));
}

void
Vector::restore_array_read(const Scalar * array) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecRestoreArrayRead(this->obj, &array));
}

void
Vector::view(PetscViewer viewer) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecView(this->obj, viewer));
}

Vector
Vector::create_seq(MPI_Comm comm, Int n)
{
    CALL_STACK_MSG();
    Vector v;
    PETSC_CHECK(VecCreateSeq(comm, n, v));
    return v;
}

Vector
Vector::create_seq(MPI_Comm comm, Int bs, Int n, const Scalar array[])
{
    CALL_STACK_MSG();
    Vector v;
    PETSC_CHECK(VecCreateSeqWithArray(comm, bs, n, array, v));
    return v;
}

Vector
Vector::create_seq(MPI_Comm comm, Int bs, const std::vector<Scalar> & data)
{
    CALL_STACK_MSG();
    Vector v;
    PETSC_CHECK(VecCreateSeqWithArray(comm, bs, data.size(), data.data(), v));
    return v;
}

Vector
Vector::create_mpi(MPI_Comm comm, Int n, Int N)
{
    CALL_STACK_MSG();
    Vector v;
    PETSC_CHECK(VecCreateMPI(comm, n, N, v));
    return v;
}

Vector
Vector::create_mpi(MPI_Comm comm, Int bs, Int n, Int N, const Scalar array[])
{
    CALL_STACK_MSG();
    Vector v;
    PETSC_CHECK(VecCreateMPIWithArray(comm, bs, n, N, array, v));
    return v;
}

Vector
Vector::create_mpi(MPI_Comm comm, Int bs, const std::vector<Scalar> & data, Int N)
{
    CALL_STACK_MSG();
    Vector v;
    PETSC_CHECK(VecCreateMPIWithArray(comm, bs, data.size(), N, data.data(), v));
    return v;
}

NestVector
Vector::create_nest(MPI_Comm comm, const std::vector<Vector> & vecs)
{
    CALL_STACK_MSG();
    std::vector<Vec> vvs;
    vvs.reserve(vecs.size());
    for (const auto & v : vecs)
        vvs.push_back(v);
    NestVector y;
    PETSC_CHECK(VecCreateNest(comm, vecs.size(), nullptr, vvs.data(), y));
    return y;
}

void
Vector::set_option(VecOption op, bool flag)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecSetOption(this->obj, op, flag ? PETSC_TRUE : PETSC_FALSE));
}

Vector
Vector::get_sub_vector(const IndexSet & is) const
{
    CALL_STACK_MSG();
    Vector y;
    PETSC_CHECK(VecGetSubVector(this->obj, is, y));
    return y;
}

void
Vector::restore_sub_vector(const IndexSet & is, Vector & y) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecRestoreSubVector(this->obj, is, &y.obj));
}

Range
Vector::get_ownership_range() const
{
    Int lo, hi;
    PETSC_CHECK(VecGetOwnershipRange(this->obj, &lo, &hi));
    return { lo, hi };
}

Real
Vector::norm(NormType type) const
{
    CALL_STACK_MSG();
    Real val;
    PETSC_CHECK(VecNorm(this->obj, type, &val));
    return val;
}

void
Vector::copy(const IndexSet & is, ScatterMode mode, Vector & reduced)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecISCopy(this->obj, is, mode, reduced));
}

Scalar
dot(const Vector & x, const Vector & y)
{
    CALL_STACK_MSG();
    Scalar val;
    PETSC_CHECK(VecDot(x, y, &val));
    return val;
}

void
copy(const Vector & x, Vector & y)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecCopy(x, y));
}

void
axpy(Vector & y, Scalar alpha, const Vector & x)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecAXPY(y, alpha, x));
}

void
axpby(Vector & y, Scalar alpha, Scalar beta, const Vector & x)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecAXPBY(y, alpha, beta, x));
}

void
aypx(Vector & y, Scalar beta, const Vector & x)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecAYPX(y, beta, x));
}

void
waxpy(Vector & w, Scalar alpha, const Vector & x, const Vector & y)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecWAXPY(w, alpha, x, y));
}

void
maxpy(Vector & y, const std::vector<Scalar> & alpha, const std::vector<Vector> & x)
{
    CALL_STACK_MSG();
    if (alpha.size() == x.size()) {
        auto n = alpha.size();
        std::vector<Vec> xx(n);
        for (std::size_t i = 0; i < n; ++i)
            xx[i] = (Vec) x[i];
        PETSC_CHECK(VecMAXPY(y, n, alpha.data(), xx.data()));
    }
    else
        throw Exception("Number of multipliers does not match the number of vectors");
}

void
axpbypcz(Vector & z, Scalar alpha, Scalar beta, Scalar gamma, const Vector & x, const Vector & y)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecAXPBYPCZ(z, alpha, beta, gamma, x, y));
}

void
pointwise_min(const Vector & w, const Vector & x, const Vector & y)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecPointwiseMin(w, x, y));
}

void
pointwise_max(const Vector & w, const Vector & x, const Vector & y)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecPointwiseMax(w, x, y));
}

void
pointwise_mult(const Vector & w, const Vector & x, const Vector & y)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecPointwiseMult(w, x, y));
}

void
pointwise_divide(const Vector & w, const Vector & x, const Vector & y)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecPointwiseDivide(w, x, y));
}

} // namespace godzilla
