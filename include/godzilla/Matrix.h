// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "godzilla/CallStack.h"
#include "godzilla/Vector.h"
#include "godzilla/DenseVector.h"
#include "godzilla/DenseMatrix.h"
#include "petscmat.h"
#include <vector>

namespace godzilla {

class ShellMatrix;

class Matrix {
public:
    enum Option {
        SPD = MAT_SPD,
        SYMMETRIC = MAT_SYMMETRIC,
        HERMITIAN = MAT_HERMITIAN,
        STRUCTURALLY_SYMMETRIC = MAT_STRUCTURALLY_SYMMETRIC,
        SYMMETRY_ETERNAL = MAT_SYMMETRY_ETERNAL,
        STRUCTURAL_SYMMETRY_ETERNAL = MAT_STRUCTURAL_SYMMETRY_ETERNAL,
        SPD_ETERNAL = MAT_SPD_ETERNAL,
        //
        NEW_NONZERO_LOCATIONS = MAT_NEW_NONZERO_LOCATIONS,
        FORCE_DIAGONAL_ENTRIES = MAT_FORCE_DIAGONAL_ENTRIES,
        NEW_NONZERO_LOCATION_ERR = MAT_NEW_NONZERO_LOCATION_ERR,
        USE_HASH_TABLE = MAT_USE_HASH_TABLE,
        NO_OFF_PROC_ENTRIES = MAT_NO_OFF_PROC_ENTRIES,
        SUBSET_OFF_PROC_ENTRIES = MAT_SUBSET_OFF_PROC_ENTRIES,
    };

    Matrix();
    Matrix(Mat mat);

    std::string get_type() const;

    /// Builds matrix object for a particular matrix type
    ///
    /// @param type The matrix type
    void set_type(const char * type);

    void set_name(const std::string & name);

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

    Scalar operator()(Int row, Int col) const;

    void view(PetscViewer viewer = PETSC_VIEWER_STDOUT_WORLD) const;

    operator Mat() const;

    static Matrix create_seq_aij(MPI_Comm comm, Int m, Int n, Int nz);
    static Matrix create_seq_aij(MPI_Comm comm, Int m, Int n, const std::vector<Int> & nnz);
    static ShellMatrix create_shell(MPI_Comm comm, Int m, Int n, Int M, Int N);

private:
    Mat mat;

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
    PETSC_CHECK(MatSetValues(this->mat, N, row_idxs.data(), N, col_idxs.data(), vals.data(), mode));
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
        MatSetValuesLocal(this->mat, N, row_idxs.data(), N, col_idxs.data(), vals.data(), mode));
}

} // namespace godzilla
