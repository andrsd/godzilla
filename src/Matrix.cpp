#include "godzilla/Matrix.h"
#include "godzilla/Error.h"

namespace godzilla {

Matrix::Matrix() : mat(nullptr) {}

Matrix::Matrix(Mat mat) : mat(mat) {}

void
Matrix::set_name(const std::string & name)
{
    _F_;
    PETSC_CHECK(PetscObjectSetName((PetscObject) this->mat, name.c_str()));
}

void
Matrix::create(MPI_Comm comm)
{
    _F_;
    PETSC_CHECK(MatCreate(comm, &this->mat));
}

void
Matrix::destroy()
{
    _F_;
    if (this->mat)
        PETSC_CHECK(MatDestroy(&this->mat));
    this->mat = nullptr;
}

void
Matrix::assembly_begin(MatAssemblyType type)
{
    _F_;
    PETSC_CHECK(MatAssemblyBegin(this->mat, type));
}

void
Matrix::assembly_end(MatAssemblyType type)
{
    _F_;
    PETSC_CHECK(MatAssemblyEnd(this->mat, type));
}

void
Matrix::get_size(Int & m, Int & n) const
{
    _F_;
    PETSC_CHECK(MatGetSize(this->mat, &m, &n));
}

Int
Matrix::get_n_rows() const
{
    _F_;
    Int rows;
    PETSC_CHECK(MatGetSize(this->mat, &rows, nullptr));
    return rows;
}

Int
Matrix::get_n_cols() const
{
    _F_;
    Int cols;
    PETSC_CHECK(MatGetSize(this->mat, nullptr, &cols));
    return cols;
}

Scalar
Matrix::get_value(Int row, Int col) const
{
    _F_;
    Scalar val;
    PETSC_CHECK(MatGetValue(this->mat, row, col, &val));
    return val;
}

void
Matrix::set_value(Int row, Int col, Scalar val, InsertMode mode)
{
    _F_;
    PETSC_CHECK(MatSetValue(this->mat, row, col, val, mode));
}

void
Matrix::set_value_local(Int row, Int col, Scalar val, InsertMode mode)
{
    _F_;
    PETSC_CHECK(MatSetValueLocal(this->mat, row, col, val, mode));
}

void
Matrix::set_values(const std::vector<Int> & row_idxs,
                   const std::vector<Int> & col_idxs,
                   const std::vector<Scalar> & vals,
                   InsertMode mode)
{
    _F_;
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
    _F_;
    MatMult(this->mat, x, y);
}

void
Matrix::zero()
{
    _F_;
    PETSC_CHECK(MatZeroEntries(this->mat));
}

Scalar
Matrix::operator()(Int row, Int col) const
{
    _F_;
    return get_value(row, col);
}

void
Matrix::view(PetscViewer viewer) const
{
    _F_;
    PETSC_CHECK(MatView(this->mat, viewer));
}

Matrix::operator Mat() const
{
    _F_;
    return this->mat;
}

Matrix
Matrix::create_seq_aij(MPI_Comm comm, Int m, Int n, Int nz)
{
    _F_;
    Matrix mat;
    PETSC_CHECK(MatCreateSeqAIJ(comm, m, n, nz, nullptr, &mat.mat));
    return mat;
}

Matrix
Matrix::create_seq_aij(MPI_Comm comm, Int m, Int n, const std::vector<Int> & nnz)
{
    _F_;
    Matrix mat;
    PETSC_CHECK(MatCreateSeqAIJ(comm, m, n, 0, nnz.data(), &mat.mat));
    return mat;
}

} // namespace godzilla
