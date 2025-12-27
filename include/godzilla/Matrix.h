// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/PetscObjectWrapper.h"
#include "godzilla/Types.h"
#include "godzilla/String.h"
#include "godzilla/CallStack.h"
#include "godzilla/Vector.h"
#include "godzilla/DenseVector.h"
#include "godzilla/DenseMatrix.h"
#include "petscmat.h"
#include <vector>

namespace godzilla {

class ShellMatrix;

class Matrix : public PetscObjectWrapper<Mat> {
public:
    enum Option {
        SYMMETRIC = MAT_SYMMETRIC,
        STRUCTURALLY_SYMMETRIC = MAT_STRUCTURALLY_SYMMETRIC,
        FORCE_DIAGONAL_ENTRIES = MAT_FORCE_DIAGONAL_ENTRIES,
        IGNORE_OFF_PROC_ENTRIES = MAT_IGNORE_OFF_PROC_ENTRIES,
        USE_HASH_TABLE = MAT_USE_HASH_TABLE,
        KEEP_NONZERO_PATTERN = MAT_KEEP_NONZERO_PATTERN,
        IGNORE_ZERO_ENTRIES = MAT_IGNORE_ZERO_ENTRIES,
        USE_INODES = MAT_USE_INODES,
        HERMITIAN = MAT_HERMITIAN,
        SYMMETRY_ETERNAL = MAT_SYMMETRY_ETERNAL,
        NEW_NONZERO_LOCATION_ERR = MAT_NEW_NONZERO_LOCATION_ERR,
        IGNORE_LOWER_TRIANGULAR = MAT_IGNORE_LOWER_TRIANGULAR,
        ERROR_LOWER_TRIANGULAR = MAT_ERROR_LOWER_TRIANGULAR,
        GETROW_UPPERTRIANGULAR = MAT_GETROW_UPPERTRIANGULAR,
        SPD = MAT_SPD,
        NO_OFF_PROC_ZERO_ROWS = MAT_NO_OFF_PROC_ZERO_ROWS,
        NO_OFF_PROC_ENTRIES = MAT_NO_OFF_PROC_ENTRIES,
        NEW_NONZERO_LOCATIONS = MAT_NEW_NONZERO_LOCATIONS,
        NEW_NONZERO_ALLOCATION_ERR = MAT_NEW_NONZERO_ALLOCATION_ERR,
        SUBSET_OFF_PROC_ENTRIES = MAT_SUBSET_OFF_PROC_ENTRIES,
        SUBMAT_SINGLEIS = MAT_SUBMAT_SINGLEIS,
        STRUCTURE_ONLY = MAT_STRUCTURE_ONLY,
        SORTED_FULL = MAT_SORTED_FULL,
        FORM_EXPLICIT_TRANSPOSE = MAT_FORM_EXPLICIT_TRANSPOSE,
        STRUCTURAL_SYMMETRY_ETERNAL = MAT_STRUCTURAL_SYMMETRY_ETERNAL,
        SPD_ETERNAL = MAT_SPD_ETERNAL,
    };

    Matrix();
    Matrix(Mat mat);

    String get_type() const;

    /// Builds matrix object for a particular matrix type
    ///
    /// @param type The matrix type
    void set_type(const char * type);

    void create(MPI_Comm comm);
    void destroy();

    void set_up();

    void assembly_begin(MatAssemblyType type = MAT_FINAL_ASSEMBLY);
    void assembly_end(MatAssemblyType type = MAT_FINAL_ASSEMBLY);
    void assemble(MatAssemblyType type = MAT_FINAL_ASSEMBLY);

    void get_size(Int & m, Int & n) const;
    Int get_n_rows() const;
    Int get_n_cols() const;

    Scalar get_value(Int row, Int col) const;

    void set_sizes(Int m, Int n, Int M = PETSC_DECIDE, Int N = PETSC_DECIDE);

    void set_value(Int row, Int col, Scalar val, InsertMode mode = INSERT_VALUES);
    void set_value_local(Int row, Int col, Scalar val, InsertMode mode = INSERT_VALUES);

    void set_values(Int n,
                    const Int * row_idxs,
                    Int m,
                    const Int * col_idxs,
                    const Scalar * vals,
                    InsertMode mode = INSERT_VALUES);

    void set_values(const std::vector<Int> & row_idxs,
                    const std::vector<Int> & col_idxs,
                    const std::vector<Scalar> & vals,
                    InsertMode mode = INSERT_VALUES);

    template <Int N>
    void set_values(const DenseVector<Int, N> & row_idxs,
                    const DenseVector<Int, N> & col_idxs,
                    const DenseMatrix<Scalar, N, N> & vals,
                    InsertMode mode = INSERT_VALUES);

    void set_values(const DynDenseVector<Int> & row_idxs,
                    const DynDenseVector<Int> & col_idxs,
                    const DynDenseMatrix<Scalar> & vals,
                    InsertMode mode = INSERT_VALUES);

    /// Inserts or adds values into certain locations of a matrix, using a local numbering of the
    /// rows and columns.
    ///
    /// @param n Number of rows
    /// @param row_idxs The row local indices
    /// @param m Number of columns
    /// @param col_idxs The column local indices
    /// @param vals The values to insert (row-major format)
    /// @param mode The insertion mode
    void set_values_local(Int n,
                          const Int * row_idxs,
                          Int m,
                          const Int * col_idxs,
                          const Scalar * vals,
                          InsertMode mode = INSERT_VALUES);

    /// Inserts or adds values into certain locations of a matrix, using a local numbering of the
    /// rows and columns.
    ///
    /// @param row_idxs The row local indices
    /// @param col_idxs The column local indices
    /// @param vals The values to insert
    /// @param mode The insertion mode
    void set_values_local(const std::vector<Int> & row_idxs,
                          const std::vector<Int> & col_idxs,
                          const std::vector<Scalar> & vals,
                          InsertMode mode = INSERT_VALUES);

    /// Inserts or adds values into certain locations of a matrix, using a local numbering of the
    /// rows and columns.
    ///
    /// @param row_idxs The row local indices
    /// @param col_idxs The column local indices
    /// @param vals The values to insert
    /// @param mode The insertion mode
    template <Int N>
    void set_values_local(const DenseVector<Int, N> & row_idxs,
                          const DenseVector<Int, N> & col_idxs,
                          const DenseMatrix<Scalar, N, N> & vals,
                          InsertMode mode = INSERT_VALUES);

    /// Inserts or adds values into certain locations of a matrix, using a local numbering of the
    /// rows and columns.
    ///
    /// @param row_idxs The row local indices
    /// @param col_idxs The column local indices
    /// @param vals The values to insert
    /// @param mode The insertion mode
    void set_values_local(const DynDenseVector<Int> & row_idxs,
                          const DynDenseVector<Int> & col_idxs,
                          const DynDenseMatrix<Scalar> & vals,
                          InsertMode mode = INSERT_VALUES);

    /// Computes the matrix-vector product, y = Ax
    void mult(const Vector & x, Vector & y);

    void zero();

    /// Scales all elements of a matrix by a given number.
    ///
    /// @param a The scaling value
    void scale(Scalar a);

    /// Computes an in-place transpose of a matrix
    void transpose();

    /// Sets a parameter option for a matrix.
    ///
    /// @param option The option to set
    /// @param flag The value of the option
    void set_option(Option option, bool flag);

    /// Sets the matrix block size.
    ///
    /// @param bs The block size
    void set_block_size(Int bs);

    /// Test whether a matrix is symmetric
    ///
    /// @param tol difference between value and its transpose less than this amount counts as equal
    ///        (use 0.0 for exact transpose)
    /// @return `true` if the matrix is symmetric, `false` otherwise
    bool is_symmetric(Real tol) const;

    Scalar operator()(Int row, Int col) const;

    void view(PetscViewer viewer = PETSC_VIEWER_STDOUT_WORLD) const;

    static Matrix create_seq_aij(MPI_Comm comm, Int m, Int n, Int nz);
    static Matrix create_seq_aij(MPI_Comm comm, Int m, Int n, const std::vector<Int> & nnz);
    static ShellMatrix create_shell(MPI_Comm comm, Int m, Int n, Int M, Int N);

private:
    friend class ShellMatrix;
};

template <Int N>
inline void
Matrix::set_values(const DenseVector<Int, N> & row_idxs,
                   const DenseVector<Int, N> & col_idxs,
                   const DenseMatrix<Scalar, N, N> & vals,
                   InsertMode mode)
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatSetValues(this->obj, N, row_idxs.data(), N, col_idxs.data(), vals.data(), mode));
}

template <Int N>
inline void
Matrix::set_values_local(const DenseVector<Int, N> & row_idxs,
                         const DenseVector<Int, N> & col_idxs,
                         const DenseMatrix<Scalar, N, N> & vals,
                         InsertMode mode)
{
    CALL_STACK_MSG();
    PETSC_CHECK(
        MatSetValuesLocal(this->obj, N, row_idxs.data(), N, col_idxs.data(), vals.data(), mode));
}

} // namespace godzilla
