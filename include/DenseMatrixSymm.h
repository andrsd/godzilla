#pragma once

#include "Error.h"
#include "Types.h"
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
public:
    DenseMatrixSymm() {};

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
        return this->data[idx(row, col)];
    }

    T &
    set(Int row, Int col)
    {
        assert((row >= 0) && (row < DIM));
        assert((col >= 0) && (col < DIM));
        return this->data[idx(row, col)];
    }

    void
    set(Int row, Int col, const T & val)
    {
        assert((row >= 0) && (row < DIM));
        assert((col >= 0) && (col < DIM));
        this->data[idx(row, col)] = val;
    }

    /// Set all matrix entries to zero, i.e. mat[i,j] = 0.
    void
    zero()
    {
        set_values(0.);
    }

    /// Set `alpha` into all matrix entries, i.e. mat[i, j] = alpha
    ///
    /// @param alpha Constant to set into vector elements
    void
    set_values(const T & alpha)
    {
        for (Int i = 0; i < N; i++)
            this->data[i] = alpha;
    }

    /// Set matrix entries from provided values
    ///
    /// @param vals Matrix entries in column major format
    void
    set_values(const std::vector<Real> & vals)
    {
        assert(vals.size() == N);
        for (Int i = 0; i < N; i++)
            this->data[i] = vals[i];
    }

    /// Multiply all entries by a scalar value
    ///
    /// @param alpha Scalar value to multiple the entries with
    void
    scale(Real alpha)
    {
        for (Int i = 0; i < N; i++)
            this->data[i] *= alpha;
    }

    /// Multiply the matrix by a vector
    ///
    /// @param x Vector to multiply by
    /// @return Resulting vector
    DenseVector<T, DIM>
    mult(const DenseVector<T, DIM> & x) const
    {
        DenseVector<T, DIM> res;
        for (Int i = 0; i < DIM; i++) {
            T prod = 0.;
            for (Int j = 0; j < DIM; j++)
                prod += get(i, j) * x(j);
            res(i) = prod;
        }
        return res;
    }

    DenseMatrix<T, DIM, DIM>
    mult(const DenseMatrixSymm<T, DIM> & x) const
    {
        DenseMatrix<T, DIM, DIM> res;
        for (Int i = 0; i < DIM; i++) {
            for (Int j = 0; j < DIM; j++) {
                T prod = 0.;
                for (Int k = 0; k < DIM; k++)
                    prod += get(i, k) * x(k, j);
                res(i, j) = prod;
            }
        }
        return res;
    }

    /// Compute determinant of the matrix
    Real
    det() const
    {
        error("Determinant is not implemented for {}x{} matrices, yet.", DIM, DIM);
    }

    /// Compute transpose
    ///
    /// @return Transposed matrix
    DenseMatrixSymm<Real, DIM>
    trans() const
    {
        return *this;
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

    T *
    get_data()
    {
        return &this->data[0];
    }

    const T *
    get_data() const
    {
        return &this->data[0];
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

    /// Number of entries of the matrix
    static const Int N = DIM * (1 + DIM) / 2;
    /// Matrix entries
    T data[N];
};

// Determinant computation for small matrices

template <>
inline Real
DenseMatrixSymm<Real, 1>::det() const
{
    return this->data[0];
}

template <>
inline Real
DenseMatrixSymm<Real, 2>::det() const
{
    return this->data[0] * this->data[2] - this->data[1] * this->data[1];
}

template <>
inline Real
DenseMatrixSymm<Real, 3>::det() const
{
    return this->data[0] * this->data[2] * this->data[5] +
           this->data[1] * this->data[4] * this->data[3] +
           this->data[1] * this->data[4] * this->data[3] -
           (this->data[3] * this->data[2] * this->data[3] +
            this->data[0] * this->data[4] * this->data[4] +
            this->data[1] * this->data[1] * this->data[5]);
}

//

template <typename T, Int N>
inline DenseMatrixSymm<T, N>
operator*(Real alpha, const DenseMatrixSymm<T, N> & a)
{
    DenseMatrixSymm<T, N> res;
    for (Int i = 0; i < N; i++)
        for (Int j = i; j < N; j++)
            res(i, j) = alpha * a(i, j);
    return res;
}

// Output

template <typename T, Int N>
std::ostream &
operator<<(std::ostream & os, const DenseMatrixSymm<T, N> & obj)
{
    for (Int i = 0; i < N; i++) {
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
