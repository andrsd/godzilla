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

template <typename T, Int ROWS, Int COLS>
class DenseMatrix;

/// Symmetrical matrix
///
/// Only upper triangular matrix is stored for memory optimization
/// Values are stored in column major
/// @tparam T Type
/// @tparam DIM Dimension of the matrix
template <typename T, Int DIM>
class DenseMatrixSymm {
private:
    /// Number of entries of the matrix
    static const Int N = DIM * (1 + DIM) / 2;

public:
    DenseMatrixSymm() = default;

    /// Create matrix from values
    ///
    /// @param vals Values in column major format
    DenseMatrixSymm(const std::vector<Real> & vals) { set_values(vals); };

    int
    get_num_rows() const
    {
        return DIM;
    }

    int
    get_num_cols() const
    {
        return DIM;
    }

    const T &
    get(Int row, Int col) const
    {
        assert((row >= 0) && (row < DIM));
        assert((col >= 0) && (col < DIM));
        return this->values[idx(row, col)];
    }

    T &
    set(Int row, Int col)
    {
        assert((row >= 0) && (row < DIM));
        assert((col >= 0) && (col < DIM));
        return this->values[idx(row, col)];
    }

    void
    set(Int row, Int col, const T & val)
    {
        assert((row >= 0) && (row < DIM));
        assert((col >= 0) && (col < DIM));
        this->values[idx(row, col)] = val;
    }

    /// Set all matrix entries to zero, i.e. mat[i,j] = 0.
    void
    zero()
    {
        zero_impl(std::is_fundamental<T>());
    }

    /// Set `alpha` into all matrix entries, i.e. mat[i, j] = alpha
    ///
    /// @param alpha Constant to set into vector elements
    void
    set_values(const T & alpha)
    {
        for (Int i = 0; i < N; ++i)
            this->values[i] = alpha;
    }

    /// Set matrix entries from provided values
    ///
    /// @param vals Matrix entries in column major format
    void
    set_values(const std::vector<Real> & vals)
    {
        assert(vals.size() == N);
        for (Int i = 0; i < N; ++i)
            this->values[i] = vals[i];
    }

    /// Multiply all entries by a scalar value
    ///
    /// @param alpha Scalar value to multiple the entries with
    void
    scale(Real alpha)
    {
        for (Int i = 0; i < N; ++i)
            this->values[i] *= alpha;
    }

    /// Add matrix `x` to this matrix
    ///
    /// @param x Matrix to add
    void
    add(const DenseMatrixSymm<T, DIM> & x)
    {
        for (Int i = 0; i < N; ++i)
            this->values[i] += x.values[i];
    }

    /// Subtract matrix `x` from this matrix
    ///
    /// @param x Matrix to subtract
    void
    subtract(const DenseMatrixSymm<T, DIM> & x)
    {
        for (Int i = 0; i < N; ++i)
            this->values[i] -= x.values[i];
    }

    /// Multiply the matrix by a vector
    ///
    /// @param x Vector to multiply by
    /// @return Resulting vector
    DenseVector<T, DIM>
    mult(const DenseVector<T, DIM> & x) const
    {
        DenseVector<T, DIM> res;
        for (Int i = 0; i < DIM; ++i) {
            T prod = 0.;
            for (Int j = 0; j < DIM; ++j)
                prod += get(i, j) * x(j);
            res(i) = prod;
        }
        return res;
    }

    DenseMatrix<T, DIM, DIM>
    mult(const DenseMatrixSymm<T, DIM> & x) const
    {
        DenseMatrix<T, DIM, DIM> res;
        for (Int i = 0; i < DIM; ++i) {
            for (Int j = 0; j < DIM; ++j) {
                T prod = 0.;
                for (Int k = 0; k < DIM; ++k)
                    prod += get(i, k) * x(k, j);
                res(i, j) = prod;
            }
        }
        return res;
    }

    template <Int M>
    DenseMatrix<T, DIM, M>
    mult(const DenseMatrix<T, DIM, M> & x) const
    {
        DenseMatrix<T, DIM, M> res;
        for (Int i = 0; i < DIM; ++i) {
            for (Int j = 0; j < M; ++j) {
                T prod = 0.;
                for (Int k = 0; k < DIM; ++k)
                    prod += get(i, k) * x(k, j);
                res(i, j) = prod;
            }
        }
        return res;
    }

    /// Compute transpose
    ///
    /// @return Transposed matrix
    DenseMatrixSymm<Real, DIM>
    trans() const
    {
        return *this;
    }

    /// Get diagonal of the matrix as a DenseVector
    ///
    /// @return Matrix diagonal as a vector
    DenseVector<Real, DIM>
    diagonal() const
    {
        DenseVector<Real, DIM> diag;
        for (Int i = 0; i < DIM; ++i)
            diag(i) = get(i, i);
        return diag;
    }

    // operators

    const T &
    operator()(Int row, Int col) const
    {
        return get(row, col);
    }

    T &
    operator()(Int row, Int col)
    {
        return set(row, col);
    }

    DenseMatrixSymm<T, DIM>
    operator-() const
    {
        DenseMatrixSymm<T, DIM> res;
        for (Int i = 0; i < N; ++i)
            res.values[i] = -this->values[i];
        return res;
    }

    /// Add matrix `a` to this matrix and return the result
    ///
    /// @param a Matrix to add
    /// @return Resulting matrix
    DenseMatrixSymm<T, DIM>
    operator+(const DenseMatrixSymm<T, DIM> & a) const
    {
        DenseMatrixSymm<T, DIM> res;
        for (Int i = 0; i < N; ++i)
            res.values[i] = this->values[i] + a.values[i];
        return res;
    }

    /// Add matrix `a` to this matrix and return the result
    ///
    /// @param a Matrix to add
    /// @return Resulting matrix
    DenseMatrix<T, DIM, DIM>
    operator+(const DenseMatrix<T, DIM, DIM> & a) const
    {
        DenseMatrix<T, DIM, DIM> res;
        for (Int i = 0; i < DIM; ++i)
            for (Int j = 0; j < DIM; ++j)
                res(i, j) = get(i, j) + a(i, j);
        return res;
    }

    /// Subtract matrix `a` from this matrix and return the result
    ///
    /// @param a Matrix to subtract
    /// @return Resulting matrix
    DenseMatrixSymm<T, DIM>
    operator-(const DenseMatrixSymm<T, DIM> & a) const
    {
        DenseMatrixSymm<T, DIM> res;
        for (Int i = 0; i < N; ++i)
            res.values[i] = this->values[i] - a.values[i];
        return res;
    }

    /// Subtract matrix `a` from this matrix and return the result
    ///
    /// @param a Matrix to subtract
    /// @return Resulting matrix
    DenseMatrix<T, DIM, DIM>
    operator-(const DenseMatrix<T, DIM, DIM> & a) const
    {
        DenseMatrix<T, DIM, DIM> res;
        for (Int i = 0; i < DIM; ++i)
            for (Int j = 0; j < DIM; ++j)
                res(i, j) = get(i, j) - a(i, j);
        return res;
    }

    /// Multiply this matrix with a scalar value
    ///
    /// @param alpha Value to multiply with
    /// @return Resulting matrix
    DenseMatrixSymm<T, DIM>
    operator*(Real alpha) const
    {
        DenseMatrixSymm<T, DIM> m(*this);
        m.scale(alpha);
        return m;
    }

    DenseVector<T, DIM>
    operator*(const DenseVector<T, DIM> & rhs) const
    {
        return mult(rhs);
    }

    DenseMatrix<T, DIM, DIM>
    operator*(const DenseMatrixSymm<T, DIM> & x) const
    {
        return mult(x);
    }

    template <Int M>
    DenseMatrix<T, DIM, M>
    operator*(const DenseMatrix<T, DIM, M> & x) const
    {
        return mult(x);
    }

    [[deprecated("Use data() instead")]] T *
    get_data()
    {
        return &this->values[0];
    }

    [[deprecated("Use data() instead")]] const T *
    get_data() const
    {
        return &this->values[0];
    }

    T *
    data()
    {
        return &this->values[0];
    }

    const T *
    data() const
    {
        return &this->values[0];
    }

    static DenseMatrixSymm<T, DIM>
    create_diagonal(const DenseVector<T, DIM> & vals)
    {
        DenseMatrixSymm<T, DIM> res;
        res.zero();
        for (Int i = 0; i < DIM; ++i)
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
        for (Int i = 0; i < N; ++i)
            this->values[i].zero();
    }

private:
    inline Int
    idx(Int row, Int col) const
    {
        if (col < row)
            return idx(col, row);
        else
            return (col * (1 + col) / 2) + row;
    }

    /// Matrix entries
    T values[N];
};

// Determinant computation for small matrices

template <typename T, Int N>
inline T
determinant(const DenseMatrixSymm<T, N> & mat)
{
    throw NotImplementedException("Determinant is not implemented for {}x{} matrices, yet", N, N);
}

template <typename T>
inline T
determinant(const DenseMatrixSymm<T, 1> & mat)
{
    return mat.data()[0];
}

template <typename T>
inline T
determinant(const DenseMatrixSymm<T, 2> & mat)
{
    const T * values = mat.data();
    return values[0] * values[2] - values[1] * values[1];
}

template <typename T>
inline T
determinant(const DenseMatrixSymm<T, 3> & mat)
{
    const T * values = mat.data();
    // clang-format off
    return values[0] * values[2] * values[5] +
           values[1] * values[4] * values[3] +
           values[1] * values[4] * values[3] -
           (values[3] * values[2] * values[3] +
            values[0] * values[4] * values[4] +
            values[1] * values[1] * values[5]);
    // clang-format on
}

//

template <typename T, Int N>
inline DenseMatrixSymm<T, N>
operator*(Real alpha, const DenseMatrixSymm<T, N> & a)
{
    DenseMatrixSymm<T, N> res;
    for (Int i = 0; i < N; ++i)
        for (Int j = i; j < N; ++j)
            res(i, j) = alpha * a(i, j);
    return res;
}

// Output

template <typename T, Int N>
std::ostream &
operator<<(std::ostream & os, const DenseMatrixSymm<T, N> & obj)
{
    for (Int i = 0; i < N; ++i) {
        os << "(";
        for (Int j = 0; j < N; ++j) {
            os << obj(i, j);
            if (j < N - 1)
                os << ", ";
        }
        os << ")" << std::endl;
    }
    return os;
}

} // namespace godzilla
