// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Matrix.h"
#include "godzilla/CallStack.h"
#include "godzilla/Error.h"
#include "godzilla/ShellMatrix.h"

namespace godzilla {

Matrix::Matrix() : PetscObjectWrapper(nullptr) {}

Matrix::Matrix(Mat mat) : PetscObjectWrapper(mat) {}

String
Matrix::get_type() const
{
    CALL_STACK_MSG();
    MatType type;
    PETSC_CHECK(MatGetType(this->obj, &type));
    return { type };
}

void
Matrix::set_type(const char * type)
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatSetType(this->obj, type));
}

void
Matrix::create(MPI_Comm comm)
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatCreate(comm, &this->obj));
}

void
Matrix::destroy()
{
    CALL_STACK_MSG();
}

void
Matrix::set_up()
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatSetUp(this->obj));
}

void
Matrix::assembly_begin(MatAssemblyType type)
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatAssemblyBegin(this->obj, type));
}

void
Matrix::assembly_end(MatAssemblyType type)
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatAssemblyEnd(this->obj, type));
}

void
Matrix::assemble(MatAssemblyType type)
{
    CALL_STACK_MSG();
    assembly_begin(type);
    assembly_end(type);
}

void
Matrix::get_size(Int & m, Int & n) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatGetSize(this->obj, &m, &n));
}

Int
Matrix::get_n_rows() const
{
    CALL_STACK_MSG();
    Int rows;
    PETSC_CHECK(MatGetSize(this->obj, &rows, nullptr));
    return rows;
}

Int
Matrix::get_n_cols() const
{
    CALL_STACK_MSG();
    Int cols;
    PETSC_CHECK(MatGetSize(this->obj, nullptr, &cols));
    return cols;
}

Scalar
Matrix::get_value(Int row, Int col) const
{
    CALL_STACK_MSG();
    Scalar val;
    PETSC_CHECK(MatGetValue(this->obj, row, col, &val));
    return val;
}

void
Matrix::set_sizes(Int m, Int n, Int M, Int N)
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatSetSizes(this->obj, m, n, M, N));
}

void
Matrix::set_value(Int row, Int col, Scalar val, InsertMode mode)
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatSetValue(this->obj, row, col, val, mode));
}

void
Matrix::set_value_local(Int row, Int col, Scalar val, InsertMode mode)
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatSetValueLocal(this->obj, row, col, val, mode));
}

void
Matrix::set_values(Int n,
                   const Int * row_idxs,
                   Int m,
                   const Int * col_idxs,
                   const Scalar * vals,
                   InsertMode mode)
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatSetValues(this->obj, n, row_idxs, m, col_idxs, vals, mode));
}

void
Matrix::set_values(const std::vector<Int> & row_idxs,
                   const std::vector<Int> & col_idxs,
                   const std::vector<Scalar> & vals,
                   InsertMode mode)
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatSetValues(this->obj,
                             (Int) row_idxs.size(),
                             row_idxs.data(),
                             (Int) col_idxs.size(),
                             col_idxs.data(),
                             vals.data(),
                             mode));
}

void
Matrix::set_values(const DynDenseVector<Int> & row_idxs,
                   const DynDenseVector<Int> & col_idxs,
                   const DynDenseMatrix<Scalar> & vals,
                   InsertMode mode)
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatSetValues(this->obj,
                             (Int) row_idxs.size(),
                             row_idxs.data(),
                             (Int) col_idxs.size(),
                             col_idxs.data(),
                             vals.data(),
                             mode));
}

void
Matrix::set_values_local(Int n,
                         const Int * row_idxs,
                         Int m,
                         const Int * col_idxs,
                         const Scalar * vals,
                         InsertMode mode)
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatSetValuesLocal(this->obj, n, row_idxs, m, col_idxs, vals, mode));
}

void
Matrix::set_values_local(const std::vector<Int> & row_idxs,
                         const std::vector<Int> & col_idxs,
                         const std::vector<Scalar> & vals,
                         InsertMode mode)
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatSetValuesLocal(this->obj,
                                  (Int) row_idxs.size(),
                                  row_idxs.data(),
                                  (Int) col_idxs.size(),
                                  col_idxs.data(),
                                  vals.data(),
                                  mode));
}

void
Matrix::set_values_local(const DynDenseVector<Int> & row_idxs,
                         const DynDenseVector<Int> & col_idxs,
                         const DynDenseMatrix<Scalar> & vals,
                         InsertMode mode)
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatSetValuesLocal(this->obj,
                                  (Int) row_idxs.size(),
                                  row_idxs.data(),
                                  (Int) col_idxs.size(),
                                  col_idxs.data(),
                                  vals.data(),
                                  mode));
}

void
Matrix::mult(const Vector & x, Vector & y)
{
    CALL_STACK_MSG();
    MatMult(this->obj, x, y);
}

void
Matrix::zero()
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatZeroEntries(this->obj));
}

void
Matrix::scale(Scalar a)
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatScale(this->obj, a));
}

void
Matrix::transpose()
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatTranspose(this->obj, MAT_INPLACE_MATRIX, &this->obj));
}

void
Matrix::set_option(Option option, bool flag)
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatSetOption(this->obj, (MatOption) option, flag ? PETSC_TRUE : PETSC_FALSE));
}

void
Matrix::set_block_size(Int bs)
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatSetBlockSize(this->obj, bs));
}

bool
Matrix::is_symmetric(Real tol) const
{
    CALL_STACK_MSG();
    PetscBool is_sym;
    PETSC_CHECK(MatIsSymmetric(this->obj, tol, &is_sym));
    return is_sym;
}

Scalar
Matrix::operator()(Int row, Int col) const
{
    CALL_STACK_MSG();
    return get_value(row, col);
}

void
Matrix::view(PetscViewer viewer) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatView(this->obj, viewer));
}

Matrix
Matrix::create_seq_aij(MPI_Comm comm, Int m, Int n, Int nz)
{
    CALL_STACK_MSG();
    Mat mat;
    PETSC_CHECK(MatCreateSeqAIJ(comm, m, n, nz, nullptr, &mat));
    return Matrix(mat);
}

Matrix
Matrix::create_seq_aij(MPI_Comm comm, Int m, Int n, const std::vector<Int> & nnz)
{
    CALL_STACK_MSG();
    Mat mat;
    PETSC_CHECK(MatCreateSeqAIJ(comm, m, n, 0, nnz.data(), &mat));
    return Matrix(mat);
}

ShellMatrix
Matrix::create_shell(MPI_Comm comm, Int m, Int n, Int M, Int N)
{
    Mat mat;
    PETSC_CHECK(MatCreateShell(comm, m, n, M, N, nullptr, &mat));
    return ShellMatrix(mat);
}

} // namespace godzilla
