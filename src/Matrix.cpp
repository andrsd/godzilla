// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Matrix.h"
#include "godzilla/Error.h"
#include "godzilla/ShellMatrix.h"

namespace godzilla {

Matrix::Matrix() : mat(nullptr) {}

Matrix::Matrix(Mat mat) : mat(mat) {}

std::string
Matrix::get_type() const
{
    CALL_STACK_MSG();
    MatType type;
    PETSC_CHECK(MatGetType(this->mat, &type));
    return { type };
}

void
Matrix::set_type(const char * type)
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatSetType(this->mat, type));
}

void
Matrix::set_name(const std::string & name)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscObjectSetName((PetscObject) this->mat, name.c_str()));
}

void
Matrix::create(MPI_Comm comm)
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatCreate(comm, &this->mat));
}

void
Matrix::destroy()
{
    CALL_STACK_MSG();
    if (this->mat)
        PETSC_CHECK(MatDestroy(&this->mat));
    this->mat = nullptr;
}

void
Matrix::set_up()
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatSetUp(this->mat));
}

void
Matrix::assembly_begin(MatAssemblyType type)
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatAssemblyBegin(this->mat, type));
}

void
Matrix::assembly_end(MatAssemblyType type)
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatAssemblyEnd(this->mat, type));
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
    PETSC_CHECK(MatGetSize(this->mat, &m, &n));
}

Int
Matrix::get_n_rows() const
{
    CALL_STACK_MSG();
    Int rows;
    PETSC_CHECK(MatGetSize(this->mat, &rows, nullptr));
    return rows;
}

Int
Matrix::get_n_cols() const
{
    CALL_STACK_MSG();
    Int cols;
    PETSC_CHECK(MatGetSize(this->mat, nullptr, &cols));
    return cols;
}

Scalar
Matrix::get_value(Int row, Int col) const
{
    CALL_STACK_MSG();
    Scalar val;
    PETSC_CHECK(MatGetValue(this->mat, row, col, &val));
    return val;
}

void
Matrix::set_sizes(Int m, Int n, Int M, Int N)
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatSetSizes(this->mat, m, n, M, N));
}

void
Matrix::set_value(Int row, Int col, Scalar val, InsertMode mode)
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatSetValue(this->mat, row, col, val, mode));
}

void
Matrix::set_value_local(Int row, Int col, Scalar val, InsertMode mode)
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatSetValueLocal(this->mat, row, col, val, mode));
}

void
Matrix::set_values(const std::vector<Int> & row_idxs,
                   const std::vector<Int> & col_idxs,
                   const std::vector<Scalar> & vals,
                   InsertMode mode)
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatSetValues(this->mat,
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
    MatMult(this->mat, x, y);
}

void
Matrix::zero()
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatZeroEntries(this->mat));
}

void
Matrix::scale(Scalar a)
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatScale(this->mat, a));
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
    PETSC_CHECK(MatView(this->mat, viewer));
}

Matrix::operator Mat() const
{
    CALL_STACK_MSG();
    return this->mat;
}

Matrix
Matrix::create_seq_aij(MPI_Comm comm, Int m, Int n, Int nz)
{
    CALL_STACK_MSG();
    Matrix mat;
    PETSC_CHECK(MatCreateSeqAIJ(comm, m, n, nz, nullptr, &mat.mat));
    return mat;
}

Matrix
Matrix::create_seq_aij(MPI_Comm comm, Int m, Int n, const std::vector<Int> & nnz)
{
    CALL_STACK_MSG();
    Matrix mat;
    PETSC_CHECK(MatCreateSeqAIJ(comm, m, n, 0, nnz.data(), &mat.mat));
    return mat;
}

ShellMatrix
Matrix::create_shell(MPI_Comm comm, Int m, Int n, Int M, Int N)
{
    Mat mat;
    PETSC_CHECK(MatCreateShell(comm, m, n, M, N, nullptr, &mat));
    return ShellMatrix(mat);
}

} // namespace godzilla
