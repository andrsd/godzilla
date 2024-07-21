// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Error.h"
#include "godzilla/Exception.h"
#include "godzilla/Types.h"
#include <cassert>
#include <vector>

namespace godzilla {

template <typename T, Int N>
class DenseVector;

template <typename T, Int N>
class DenseMatrixSymm;

/// Dense matrix with `ROWS` rows and `COLS` columns
///
/// Entries are stored in row-major format
/// @tparam T Data type of matrix entries
/// @tparam ROWS Number of rows
/// @tparam COLS Number of columns
template <typename T, Int ROWS, Int COLS = ROWS>
class DenseMatrix {
public:
    DenseMatrix() = default;

    DenseMatrix(const DenseMatrixSymm<T, ROWS> & m)
    {
        assert(COLS == ROWS);
        for (Int i = 0; i < ROWS; i++)
            for (Int j = 0; j < COLS; j++)
                set(i, j) = m(i, j);
    }

    /// Get the number of rows
    ///
    /// @return The number of rows
    [[nodiscard]] int
    get_num_rows() const
    {
        return ROWS;
    }

    /// Get the number of columns
    ///
    /// @return The number of columns
    [[nodiscard]] int
    get_num_cols() const
    {
        return COLS;
    }

    /// Get entry at specified location for reading
    ///
    /// @param row Row number
    /// @param col Column number
    /// @return Entry at the specified location
    const T &
    get(Int row, Int col) const
    {
        assert((row >= 0) && (row < ROWS));
        assert((col >= 0) && (col < COLS));
        return this->values[idx(row, col)];
    }

    /// Obtain a column from the matrix
    ///
    /// @param idx Index of the column
    /// @return Column as a DenseVector
    DenseMatrix<T, ROWS, 1>
    column(Int idx) const
    {
        DenseMatrix<T, ROWS, 1> col;
        for (Int row = 0; row < ROWS; row++)
            col(row, 0) = get(row, idx);
        return col;
    }

    /// Obtain a column from the matrix
    ///
    /// @param idx Index of the column
    /// @return Column as a DenseVector
    DenseMatrix<T, 1, COLS>
    row(Int idx) const
    {
        DenseMatrix<T, 1, COLS> row;
        for (Int col = 0; col < COLS; col++)
            row(0, col) = get(idx, col);
        return row;
    }

    /// Get entry at specified location for writing
    ///
    /// @param row Row number
    /// @param col Column number
    /// @return Entry at the specified location
    T &
    set(Int row, Int col)
    {
        assert((row >= 0) && (row < ROWS));
        assert((col >= 0) && (col < COLS));
        return this->values[idx(row, col)];
    }

    /// Set value at a specified location
    ///
    /// @param row Row number
    /// @param col Column number
    /// @param val Value to set at position (row, col)
    void
    set(Int row, Int col, const T & val)
    {
        assert((row >= 0) && (row < ROWS));
        assert((col >= 0) && (col < COLS));
        this->values[idx(row, col)] = val;
    }

    /// Set a matrix row at once
    ///
    /// This is a convenience method
    /// @param row Row number
    /// @param vals Values of the row. `vals` must have `COLS` values.
    void
    set_row(Int row, const std::vector<T> & vals)
    {
        assert(vals.size() == COLS);
        for (Int i = 0; i < COLS; i++)
            set(row, i) = vals[i];
    }

    /// Set a matrix row at once
    ///
    /// This is a convenience method
    /// @param row Row number
    /// @param vals Values of the row
    void
    set_row(Int row, const DenseVector<T, COLS> & vals)
    {
        for (Int i = 0; i < COLS; i++)
            set(row, i) = vals(i);
    }

    void
    set_row(Int row, const DenseMatrix<T, 1, COLS> & vals)
    {
        for (Int i = 0; i < COLS; i++)
            set(row, i) = vals(0, i);
    }

    void
    set_col(Int col, const std::vector<T> & vals)
    {
        assert(vals.size() == ROWS);
        for (Int i = 0; i < ROWS; i++)
            set(i, col) = vals[i];
    }

    /// Set a matrix column at once
    ///
    /// This is a convenience method
    /// @param col Column number
    /// @param vals Values of the column
    void
    set_col(Int col, const DenseVector<T, ROWS> & vals)
    {
        for (Int i = 0; i < ROWS; i++)
            set(i, col) = vals(i);
    }

    /// Set all matrix entries to zero, i.e. mat[i,j] = 0.
    void
    zero()
    {
        zero_impl(std::is_fundamental<T>());
    }

    /// Set `alpha` into all matrix entries, i.e. mat[i, j] = alpha
    ///
    /// @param alpha Value to set into matrix entries
    void
    set_values(const T & alpha)
    {
        for (Int i = 0; i < ROWS * COLS; i++)
            this->values[i] = alpha;
    }

    /// Multiply all entries by a scalar value
    ///
    /// @param alpha Value to multiple the matrix entries with
    void
    scale(Real alpha)
    {
        for (Int i = 0; i < ROWS; i++)
            for (Int j = 0; j < COLS; j++)
                set(i, j) *= alpha;
    }

    /// Add matrix `x` to this matrix
    ///
    /// @param x Matrix to add
    void
    add(const DenseMatrix<T, ROWS, COLS> & x)
    {
        for (Int i = 0; i < ROWS; i++)
            for (Int j = 0; j < COLS; j++)
                set(i, j) += x.get(i, j);
    }

    /// Add matrix `x` to this matrix
    ///
    /// @param x Matrix to add
    void
    add(const DenseMatrixSymm<T, ROWS> & x)
    {
        assert(ROWS == COLS);
        for (Int i = 0; i < ROWS; i++)
            for (Int j = 0; j < COLS; j++)
                set(i, j) += x.get(i, j);
    }

    /// Subtract matrix `x` from this matrix
    ///
    /// @param x Matrix to subtract
    void
    subtract(const DenseMatrix<T, ROWS, COLS> & x)
    {
        for (Int i = 0; i < ROWS; i++)
            for (Int j = 0; j < COLS; j++)
                set(i, j) -= x.get(i, j);
    }

    /// Subtract matrix `x` from this matrix
    ///
    /// @param x Matrix to subtract
    void
    subtract(const DenseMatrixSymm<T, ROWS> & x)
    {
        assert(ROWS == COLS);
        for (Int i = 0; i < ROWS; i++)
            for (Int j = 0; j < COLS; j++)
                set(i, j) -= x.get(i, j);
    }

    /// Multiply the matrix by a vector
    ///
    /// Vector size must be equal to the number of columns
    /// @param x Vector to multiply by
    /// @return Resulting vector
    DenseVector<T, ROWS>
    mult(const DenseVector<T, COLS> & x) const
    {
        DenseVector<T, ROWS> res;
        for (Int i = 0; i < ROWS; i++) {
            T prod = 0.;
            for (Int j = 0; j < COLS; j++)
                prod += get(i, j) * x(j);
            res(i) = prod;
        }
        return res;
    }

    template <Int ROWS2>
    DenseMatrix<T, ROWS, ROWS2>
    mult(const DenseMatrix<T, COLS, ROWS2> & x) const
    {
        DenseMatrix<T, ROWS, ROWS2> res;
        for (Int i = 0; i < ROWS; i++) {
            for (Int j = 0; j < ROWS2; j++) {
                T prod = 0.;
                for (Int k = 0; k < COLS; k++)
                    prod += get(i, k) * x(k, j);
                res(i, j) = prod;
            }
        }
        return res;
    }

    DenseMatrix<T, ROWS, COLS>
    mult(const DenseMatrixSymm<T, COLS> & x) const
    {
        DenseMatrix<T, ROWS, COLS> res;
        for (Int i = 0; i < ROWS; i++) {
            for (Int j = 0; j < COLS; j++) {
                T prod = 0.;
                for (Int k = 0; k < COLS; k++)
                    prod += get(i, k) * x(k, j);
                res(i, j) = prod;
            }
        }
        return res;
    }

    /// Compute transpose
    ///
    /// @return Transposed matrix
    [[deprecated("Use transpose() instead")]] DenseMatrix<T, COLS, ROWS>
    trans() const
    {
        return transpose();
    }

    /// Compute transpose
    ///
    /// @return Transposed matrix
    DenseMatrix<T, COLS, ROWS>
    transpose() const
    {
        DenseMatrix<T, COLS, ROWS> tr;
        for (Int i = 0; i < ROWS; i++)
            for (Int j = 0; j < COLS; j++)
                tr(j, i) = get(i, j);
        return tr;
    }

    /// Get diagonal of the matrix as a DenseVector
    ///
    /// @return Matrix diagonal as a vector
    DenseVector<T, COLS>
    diagonal() const
    {
        assert(ROWS == COLS);
        DenseVector<T, COLS> diag;
        for (Int i = 0; i < COLS; i++)
            diag(i) = get(i, i);
        return diag;
    }

    // operators

    /// Get a value at position (row, col) for reading
    ///
    /// @param row Row number
    /// @param col Col number
    /// @return Value at the position (row, col)
    const T &
    operator()(Int row, Int col) const
    {
        return get(row, col);
    }

    /// Get a value at position (row, col) for writing
    ///
    /// @param row Row number
    /// @param col Col number
    /// @return Value at position (row, col)
    T &
    operator()(Int row, Int col)
    {
        return set(row, col);
    }

    DenseMatrix<T, ROWS, COLS>
    operator-() const
    {
        DenseMatrix<T, ROWS, COLS> res;
        for (Int i = 0; i < ROWS * COLS; i++)
            res.values[i] = -this->values[i];
        return res;
    }

    /// Add matrix `a` to this matrix and return the result
    ///
    /// @param a Matrix to add
    /// @return Resulting matrix
    DenseMatrix<T, ROWS, COLS>
    operator+(const DenseMatrix<T, ROWS, COLS> & a) const
    {
        DenseMatrix<T, ROWS, COLS> res;
        for (Int i = 0; i < ROWS; i++)
            for (Int j = 0; j < COLS; j++)
                res(i, j) = this->get(i, j) + a.get(i, j);
        return res;
    }

    /// Add matrix `a` to this matrix and return the result
    ///
    /// This matrix must be a square matrix
    /// @param a Matrix to add
    /// @return Resulting matrix
    DenseMatrix<T, ROWS>
    operator+(const DenseMatrixSymm<T, ROWS> & a) const
    {
        assert(ROWS == COLS);
        DenseMatrix<T, ROWS> res;
        for (Int i = 0; i < ROWS; i++)
            for (Int j = 0; j < COLS; j++)
                res(i, j) = this->get(i, j) + a.get(i, j);
        return res;
    }

    /// Add matrix to this matrix
    ///
    /// @param a Matrix to add
    /// @return Resulting matrix, i.e. `this + a`
    DenseMatrix<T, ROWS, COLS> &
    operator+=(const DenseMatrix<T, ROWS, COLS> & a)
    {
        for (Int i = 0; i < ROWS; i++)
            for (Int j = 0; j < COLS; j++)
                set(i, j) += a.get(i, j);
        return *this;
    }

    /// Subtract matrix `a` from this matrix and return the result
    ///
    /// @param a Matrix to subtract
    /// @return Resulting matrix
    DenseMatrix<T, ROWS, COLS>
    operator-(const DenseMatrix<T, ROWS, COLS> & a) const
    {
        DenseMatrix<T, ROWS, COLS> res;
        for (Int i = 0; i < ROWS; i++)
            for (Int j = 0; j < COLS; j++)
                res(i, j) = this->get(i, j) - a.get(i, j);
        return res;
    }

    /// Subtract matrix `a` from this matrix and return the result
    ///
    /// This matrix must be a square matrix
    /// @param a Matrix to subtract
    /// @return Resulting matrix
    DenseMatrix<T, ROWS>
    operator-(const DenseMatrixSymm<T, ROWS> & a) const
    {
        assert(ROWS == COLS);
        DenseMatrix<T, ROWS> res;
        for (Int i = 0; i < ROWS; i++)
            for (Int j = 0; j < COLS; j++)
                res(i, j) = this->get(i, j) - a.get(i, j);
        return res;
    }

    /// Multiply this matrix with a scalar value
    ///
    /// @param alpha Value to multiply with
    /// @return Resulting matrix
    DenseMatrix<T, ROWS, COLS>
    operator*(Real alpha) const
    {
        DenseMatrix<T, ROWS, COLS> m(*this);
        m.scale(alpha);
        return m;
    }

    /// Multiply this matrix with a vector
    ///
    /// @param rhs Vector to multiply with
    /// @return Resulting vector
    DenseVector<T, ROWS>
    operator*(const DenseVector<T, COLS> & rhs) const
    {
        return mult(rhs);
    }

    template <Int ROWS2>
    DenseMatrix<T, ROWS, ROWS2>
    operator*(const DenseMatrix<T, COLS, ROWS2> & x) const
    {
        return mult(x);
    }

    DenseMatrix<T, ROWS, COLS>
    operator*(const DenseMatrixSymm<T, COLS> & x) const
    {
        return mult(x);
    }

    DenseMatrix<T, ROWS> &
    operator=(const DenseMatrixSymm<T, ROWS> & m)
    {
        assert(COLS == ROWS);
        for (Int i = 0; i < ROWS; i++)
            for (Int j = 0; j < COLS; j++)
                set(i, j) = m(i, j);
        return *this;
    }

    /// Get access to the underlying data
    ///
    /// WARNING: Avoid using this API as much as you can
    /// @return Pointer to the underlying matrix entries
    [[deprecated("Use data() instead")]] T *
    get_data()
    {
        return &this->values[0];
    }

    /// Get access to the underlying data
    ///
    /// WARNING: Avoid using this API as much as you can
    /// @return Pointer to the underlying matrix entries
    [[deprecated("Use data() instead")]] const T *
    get_data() const
    {
        return &this->values[0];
    }

    /// Get access to the underlying data
    ///
    /// WARNING: Avoid using this API as much as you can
    /// @return Pointer to the underlying matrix entries
    T *
    data()
    {
        return &this->values[0];
    }

    /// Get access to the underlying data
    ///
    /// WARNING: Avoid using this API as much as you can
    /// @return Pointer to the underlying matrix entries
    const T *
    data() const
    {
        return &this->values[0];
    }

    static DenseMatrix<T, ROWS, ROWS>
    create_symm(const std::vector<T> & vals)
    {
        assert(vals.size() == (ROWS * (ROWS + 1) / 2));
        DenseMatrix<T, ROWS, ROWS> res;
        // store upper triangular
        for (Int i = 0, k = 0; i < ROWS; i++)
            for (Int j = i; j < ROWS; j++, k++)
                res(i, j) = vals[k];
        // create lower triangular
        for (Int i = 0; i < ROWS; i++)
            for (Int j = 0; j < i; j++)
                res(i, j) = res(j, i);
        return res;
    }

    static DenseMatrix<T, ROWS, ROWS>
    create_diagonal(const DenseVector<T, ROWS> & vals)
    {
        DenseMatrix<T, ROWS, ROWS> res;
        res.zero();
        for (Int i = 0; i < ROWS; i++)
            res(i, i) = vals(i);
        return res;
    }

protected:
    void
    zero_impl(std::true_type)
    {
        set_values(0);
    }

    void
    zero_impl(std::false_type)
    {
        for (Int i = 0; i < ROWS * COLS; i++)
            this->values[i].zero();
    }

private:
    /// Mapping function from (row, col) to the offset into the internal array that stores the
    /// matrix entries
    ///
    /// @param row Row number
    /// @param col Column number
    /// @return Offset into the `values` array that contains the entry at position (row, col)
    [[nodiscard]] Int
    idx(Int row, Int col) const
    {
        return row * COLS + col;
    }

    /// Array that stores the matrix entries
    T values[ROWS * COLS];
};

// ---

template <typename T, Int N>
inline T
determinant(const DenseMatrix<T, N, N> & mat)
{
    error("Determinant is not implemented for {}x{} matrices, yet.", N, N);
}

template <typename T>
inline T
determinant(const DenseMatrix<T, 1, 1> & mat)
{
    return mat.data()[0];
}

template <typename T>
inline T
determinant(const DenseMatrix<T, 2, 2> & mat)
{
    const T * values = mat.data();
    return values[0] * values[3] - values[2] * values[1];
}

template <typename T>
inline T
determinant(const DenseMatrix<T, 3, 3> & mat)
{
    const T * values = mat.data();
    return values[0] * values[4] * values[8] + values[3] * values[7] * values[2] +
           values[1] * values[5] * values[6] -
           (values[6] * values[4] * values[2] + values[1] * values[3] * values[8] +
            values[5] * values[7] * values[0]);
}

// Inversion

/// Compute inverse of this matrix
///
/// @return Inverse of this matrix
template <typename T, Int N>
inline DenseMatrix<T, N>
inverse(const DenseMatrix<T, N> & mat)
{
    error("Inverse is not implemented for {}x{} matrices, yet.", N, N);
}

template <>
inline DenseMatrix<Real, 1>
inverse(const DenseMatrix<Real, 1> & mat)
{
    DenseMatrix<Real, 1> inv;
    inv(0, 0) = 1. / mat.data()[0];
    return inv;
}

template <>
inline DenseMatrix<Real, 2>
inverse(const DenseMatrix<Real, 2> & mat)
{
    Real det = determinant(mat);
    if (det == 0.)
        throw Exception("Inverting of a matrix failed: matrix is singular.");

    DenseMatrix<Real, 2> inv;
    inv.data()[0] = mat.data()[3];
    inv.data()[1] = -mat.data()[1];
    inv.data()[2] = -mat.data()[2];
    inv.data()[3] = mat.data()[0];
    inv.scale(1. / det);
    return inv;
}

template <>
inline DenseMatrix<Real, 3>
inverse(const DenseMatrix<Real, 3> & mat)
{
    Real det = determinant(mat);
    if (det == 0.)
        throw Exception("Inverting of a matrix failed: matrix is singular.");

    DenseMatrix<Real, 3> inv;
    inv(0, 0) = (mat.data()[4] * mat.data()[8] - mat.data()[5] * mat.data()[7]);
    inv(1, 0) = -(mat.data()[3] * mat.data()[8] - mat.data()[5] * mat.data()[6]);
    inv(2, 0) = (mat.data()[3] * mat.data()[7] - mat.data()[4] * mat.data()[6]);
    inv(0, 1) = -(mat.data()[1] * mat.data()[8] - mat.data()[2] * mat.data()[7]);
    inv(1, 1) = (mat.data()[0] * mat.data()[8] - mat.data()[2] * mat.data()[6]);
    inv(2, 1) = -(mat.data()[0] * mat.data()[7] - mat.data()[1] * mat.data()[6]);
    inv(0, 2) = (mat.data()[1] * mat.data()[5] - mat.data()[2] * mat.data()[4]);
    inv(1, 2) = -(mat.data()[0] * mat.data()[5] - mat.data()[2] * mat.data()[3]);
    inv(2, 2) = (mat.data()[0] * mat.data()[4] - mat.data()[1] * mat.data()[3]);
    inv.scale(1. / det);
    return inv;
}

/// Transpose DenseMatrix<T, M, N>
///
/// @tparam T Data type
/// @tparam N Number of rows in the input "matrix", but number of columns in the resulting matrix
/// @tparam M Number of columns in the input "matrix", but number of rows in the resulting matrix
/// @param a Input "matrix"
/// @return Transposed version of DenseMatrix<T> with values from `a`
template <typename T, Int N, Int M>
inline DenseMatrix<T, N, M>
transpose(const DenseMatrix<T, M, N> & a)
{
    DenseMatrix<T, N, M> res;
    for (Int i = 0; i < N; i++)
        for (Int j = 0; j < M; j++)
            res(i, j) = a(j, i);
    return res;
}

//

template <typename T, Int M, Int N = M>
inline DenseMatrix<T, M, N>
operator*(Real alpha, const DenseMatrix<T, M, N> & a)
{
    DenseMatrix<T, M, N> res;
    for (Int i = 0; i < M; i++)
        for (Int j = 0; j < N; j++)
            res(i, j) = alpha * a(i, j);
    return res;
}

// Output

template <typename T, Int M, Int N>
std::ostream &
operator<<(std::ostream & os, const DenseMatrix<T, M, N> & obj)
{
    for (Int i = 0; i < M; i++) {
        os << "(";
        for (Int j = 0; j < N; j++) {
            os << obj(i, j);
            if (j < N - 1)
                os << ", ";
        }
        os << ")" << std::endl;
    }
    return os;
}

} // namespace godzilla
