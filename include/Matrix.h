#pragma once

#include "Types.h"
#include "CallStack.h"
#include "Vector.h"
#include "DenseVector.h"
#include "DenseMatrix.h"
#include "petscmat.h"
#include <vector>

namespace godzilla {

class Matrix {
public:
    Matrix();
    Matrix(Mat mat);

    void set_name(const std::string & name);

    void create(MPI_Comm comm);
    void destroy();

    void assembly_begin(MatAssemblyType type = MAT_FINAL_ASSEMBLY);
    void assembly_end(MatAssemblyType type = MAT_FINAL_ASSEMBLY);

    void get_size(Int & m, Int & n) const;
    Int get_n_rows() const;
    Int get_n_cols() const;

    Scalar get_value(Int row, Int col) const;

    void set_value(Int row, Int col, Scalar val, InsertMode mode = INSERT_VALUES);
    void set_value_local(Int row, Int col, Scalar val, InsertMode mode = INSERT_VALUES);

    void set_values(const std::vector<Int> & row_idxs,
                    const std::vector<Int> & col_idxs,
                    const std::vector<Scalar> & vals,
                    InsertMode mode = INSERT_VALUES);

    template <Int N>
    void set_values(const DenseVector<Int, N> & row_idxs,
                    const DenseVector<Int, N> & col_idxs,
                    const DenseMatrix<Scalar, N, N> & vals,
                    InsertMode mode = INSERT_VALUES);

    /// Computes the matrix-vector product, y = Ax
    void mult(const Vector & x, Vector & y);

    void zero();

    Scalar operator()(Int row, Int col) const;

    void view(PetscViewer viewer = PETSC_VIEWER_STDOUT_WORLD) const;

    operator Mat() const;

    static Matrix create_seq_aij(MPI_Comm comm, Int m, Int n, Int nz);
    static Matrix create_seq_aij(MPI_Comm comm, Int m, Int n, const std::vector<Int> & nnz);

private:
    Mat mat;
};

template <Int N>
inline void
Matrix::set_values(const DenseVector<Int, N> & row_idxs,
                   const DenseVector<Int, N> & col_idxs,
                   const DenseMatrix<Scalar, N, N> & vals,
                   InsertMode mode)
{
    _F_;
    PETSC_CHECK(
        MatSetValues(this->mat, N, row_idxs.data(), N, col_idxs.data(), vals.data(), mode));
}

} // namespace godzilla
