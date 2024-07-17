// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "godzilla/Error.h"
#include "godzilla/DenseMatrix.h"
#include <cassert>

namespace godzilla {

template <typename T, Int ROWS>
class DenseMatrixSymm;

/// Dense vector with `N` entries
///
/// @tparam T Data type of matrix entries
/// @tparam N Number of entries
template <typename T, Int N>
class DenseVector : public DenseMatrix<T, N, 1> {
public:
    /// Create empty vector
    DenseVector() = default;

    /// Create vector from a std::vector
    ///
    /// @param a Values to initialize the vector with
    explicit DenseVector(const std::vector<T> & a)
    {
        for (Int i = 0; i < N; i++)
            set(i) = a[i];
    }

    DenseVector(const DenseMatrix<T, N, 1> & a)
    {
        for (Int i = 0; i < N; i++)
            set(i) = a(i, 0);
    }

    /// Get an entry at location (i) for reading
    ///
    /// @param i Index
    /// @return Entry at location (i)
    [[nodiscard]] const T &
    get(Int i) const
    {
        assert((i >= 0) && (i < N));
        return DenseMatrix<T, N, 1>::get(i, 0);
    }

    /// Get an entry at location (i) for writing
    ///
    /// @param i Index
    /// @return Entry at location (i)
    T &
    set(Int i)
    {
        assert((i >= 0) && (i < N));
        return DenseMatrix<T, N, 1>::set(i, 0);
    }

    /// Add `a` to this vector, i.e. vec[i] += a[i]
    ///
    /// @param a Vector to add
    void
    add(const DenseVector<T, N> & a)
    {
        for (Int i = 0; i < N; i++)
            set(i) += a(i);
    }

    /// Add `a` to each element of this vector, i.e. vec[i] += a
    ///
    /// @param a Value to add
    void
    add(T a)
    {
        for (Int i = 0; i < N; i++)
            set(i) += a;
    }

    /// Subtract `a` from this vector, i.e. vec[i] -= a[i]
    ///
    /// @param a Vector to subtract
    void
    subtract(const DenseVector<T, N> & a)
    {
        for (Int i = 0; i < N; i++)
            set(i) -= a(i);
    }

    /// Normalize this vector
    void
    normalize()
    {
        T mag = magnitude();
        if (mag > 0) {
            for (Int i = 0; i < N; i++)
                set(i) /= mag;
        }
    }

    /// Compute average from vector entries
    ///
    /// @return Average of vector entries
    [[nodiscard]] Real
    avg() const
    {
        Real res = 0.;
        for (Int i = 0; i < N; i++)
            res += get(i);
        return res / N;
    }

    [[nodiscard]] DenseVector<Real, N>
    cross(const DenseVector<Real, N> &) const
    {
        error("Cross product in {} dimensions is not unique.", N);
    }

    template <Int M>
    DenseMatrix<Real, N, M>
    tensor_prod(const DenseVector<Real, M> & a) const
    {
        DenseMatrix<Real, N, M> res;
        for (Int i = 0; i < N; i++)
            for (Int j = 0; j < M; j++)
                res(i, j) = get(i) * a(j);
        return res;
    }

    /// Sum all vector elements, i.e \Sum_i vec[i]
    ///
    /// @return Sum of all elements
    [[nodiscard]] T
    sum() const
    {
        T sum = 0.;
        for (Int i = 0; i < N; i++)
            sum += get(i);
        return sum;
    }

    /// Compute vector magnitude, i.e. sqrt(\Sum_i vec[i]^2)
    ///
    /// @return Vector magnitude
    [[nodiscard]] T
    magnitude() const
    {
        T sum = 0.;
        for (Int i = 0; i < N; i++)
            sum += get(i) * get(i);
        return std::sqrt(sum);
    }

    /// Point-wise division of this vector with another one
    ///
    /// @param b Second vector
    /// @return Vector with entries this[i]/b[i]
    [[nodiscard]] DenseVector<T, N>
    pointwise_div(const DenseVector<T, N> & b) const
    {
        DenseVector<T, N> res;
        for (Int i = 0; i < N; i++)
            res(i) = get(i) / b(i);
        return res;
    }

    /// Find the minimum value of the elements
    ///
    /// @return The minimum value of the elements
    [[nodiscard]] T
    min() const
    {
        T res = std::numeric_limits<T>::max();
        for (Int i = 0; i < N; i++)
            if (get(i) < res)
                res = get(i);
        return res;
    }

    /// Find the minimum value of the elements
    ///
    /// @return The minimum value of the elements
    [[nodiscard]] T
    max() const
    {
        T res = std::numeric_limits<T>::min();
        for (Int i = 0; i < N; i++)
            if (get(i) > res)
                res = get(i);
        return res;
    }

    /// Replaces every element in a vector with its absolute value
    void
    abs()
    {
        for (Int i = 0; i < N; i++)
            set(i) = std::abs(get(i));
    }

    // Operators

    const T &
    operator()(Int i) const
    {
        return get(i);
    }

    T &
    operator()(Int i)
    {
        return set(i);
    }

    DenseVector<T, N>
    operator+(const DenseVector<T, N> & a) const
    {
        DenseVector<T, N> res;
        for (Int i = 0; i < N; i++)
            res(i) = get(i) + a(i);
        return res;
    }

    DenseVector<T, N>
    operator-() const
    {
        DenseVector<T, N> res;
        for (Int i = 0; i < N; i++)
            res(i) = -get(i);
        return res;
    }

    DenseVector<T, N>
    operator-(const DenseVector<T, N> & a) const
    {
        DenseVector<T, N> res;
        for (Int i = 0; i < N; i++)
            res(i) = get(i) - a(i);
        return res;
    }

    template <Int M>
    DenseMatrix<T, N, M>
    operator*(const DenseMatrix<T, 1, M> & a) const
    {
        DenseMatrix<T, N, M> res;
        for (Int i = 0; i < M; i++)
            for (Int j = 0; j < N; j++)
                res(j, i) = get(j) * a(0, i);
        return res;
    }

    DenseVector<T, N>
    operator*(Real alpha) const
    {
        DenseVector<T, N> res;
        for (Int i = 0; i < N; i++)
            res(i) = alpha * get(i);
        return res;
    }

    DenseVector<T, N> &
    operator+=(const DenseVector<T, N> & a)
    {
        for (Int i = 0; i < N; i++)
            set(i) += a(i);
        return *this;
    }

    DenseVector<T, N> &
    operator+=(const T & a)
    {
        for (Int i = 0; i < N; i++)
            set(i) += a;
        return *this;
    }

    DenseVector<T, N> &
    operator-=(const DenseVector<T, N> & a)
    {
        for (Int i = 0; i < N; i++)
            set(i) -= a(i);
        return *this;
    }
};

/// Compute dot product of 2 column-vectors
///
/// @tparam T Data type
/// @tparam N Size of the vector
/// @param a First column-vector
/// @param b Second column-vector
/// @return Dot product
template <typename T, Int N>
inline T
dot(const DenseMatrix<T, N, 1> & a, const DenseMatrix<T, N, 1> & b)
{
    T dot = 0.;
    for (Int i = 0; i < N; i++)
        dot += a(i, 0) * b(i, 0);
    return dot;
}

/// Compute dot product of 2 vectors
///
/// @tparam T Data type
/// @tparam N Size of the vector
/// @param a First row-vector
/// @param b Second column-vector
/// @return Dot product
template <typename T, Int N>
inline T
dot(const DenseMatrix<T, 1, N> & a, const DenseMatrix<T, N, 1> & b)
{
    T dot = 0.;
    for (Int i = 0; i < N; i++)
        dot += a(0, i) * b(i, 0);
    return dot;
}

/// Compute dot product of 2 row-vectors
///
/// @tparam T Data type
/// @tparam N Size of the row-vector
/// @param a First row-vector
/// @param b Second row-vector
/// @return Dot product
template <typename T, Int N>
inline T
dot(const DenseMatrix<T, 1, N> & a, const DenseMatrix<T, 1, N> & b)
{
    T dot = 0.;
    for (Int i = 0; i < N; i++)
        dot += a(0, i) * b(0, i);
    return dot;
}

/// Dot product of 2 "scalars"
///
/// NOTE: This exists to avoid ambiguity between DenseMatrix<T, N, 1> and DenseMatrix<T, 1, N> for N
/// = 1
template <typename T>
inline T
dot(const DenseMatrix<T, 1, 1> & a, const DenseMatrix<T, 1, 1> & b)
{
    return a(0, 0) * b(0, 0);
}

/// Pointwise multiplication of 2 column-vectors
///
/// @tparam T Data type
/// @tparam N Size of the column-vector
/// @param a First column-vector
/// @param b Second column-vector
/// @return column-vector with vec[i] = a[i] * b[i]
template <typename T, Int N>
inline DenseMatrix<T, N, 1>
pointwise_mult(const DenseMatrix<T, N, 1> & a, const DenseMatrix<T, N, 1> & b)
{
    DenseMatrix<T, N, 1> res;
    for (Int i = 0; i < N; i++)
        res(i, 0) = a(i, 0) * b(i, 0);
    return res;
}

/// Pointwise multiplication of 2 row-vectors
///
/// @tparam T Data type
/// @tparam N Size of the row-vector
/// @param a First row-vector
/// @param b Second row-vector
/// @return Row-vector with vec[i] = a[i] * b[i]
template <typename T, Int N>
inline DenseMatrix<T, 1, N>
pointwise_mult(const DenseMatrix<T, 1, N> & a, const DenseMatrix<T, 1, N> & b)
{
    DenseMatrix<T, 1, N> res;
    for (Int i = 0; i < N; i++)
        res(0, i) = a(0, i) * b(0, i);
    return res;
}

/// Pointwise division of 2 vectors
///
/// @tparam T Data type
/// @tparam N Size of the vector
/// @param a First vector
/// @param b Second vector
/// @return Vector with vec[i] = a[i] / b[i]
template <typename T, Int N>
inline DenseVector<T, N>
pointwise_div(const DenseVector<T, N> & a, const DenseVector<T, N> & b)
{
    DenseVector<T, N> res;
    for (Int i = 0; i < N; i++)
        res(i) = a(i) / b(i);
    return res;
}

template <typename T, Int N>
inline DenseVector<T, N>
operator*(Real alpha, const DenseVector<T, N> & a)
{
    DenseVector<T, N> res;
    for (Int i = 0; i < N; i++)
        res(i) = alpha * a(i);
    return res;
}

// Cross product

template <>
inline DenseVector<Real, 1>
DenseVector<Real, 1>::cross(const DenseVector<Real, 1> &) const
{
    error("Cross product of 1D vectors is not defined.");
}

template <>
inline DenseVector<Real, 2>
DenseVector<Real, 2>::cross(const DenseVector<Real, 2> &) const
{
    error("Cross product of 2D vectors is not defined.");
}

template <>
inline DenseVector<Real, 3>
DenseVector<Real, 3>::cross(const DenseVector<Real, 3> & a) const
{
    DenseVector<Real, 3> res;
    res(0) = get(1) * a(2) - get(2) * a(1);
    res(1) = -(get(0) * a(2) - get(2) * a(0));
    res(2) = get(0) * a(1) - get(1) * a(0);
    return res;
}

/// Compute cross product from 2 vectors
///
/// @param a First vector
/// @param b Second vector
/// @return Resulting vector \f$ a x b\f$
inline DenseVector<Real, 3>
cross_product(const DenseVector<Real, 3> & a, const DenseVector<Real, 3> & b)
{
    DenseVector<Real, 3> res;
    res(0) = a(1) * b(2) - a(2) * b(1);
    res(1) = -(a(0) * b(2) - a(2) * b(0));
    res(2) = a(0) * b(1) - a(1) * b(0);
    return res;
}

template <typename T, Int M, Int N>
inline DenseMatrix<T, M, N>
tensor_product(const DenseVector<T, M> & a, const DenseVector<T, N> & b)
{
    DenseMatrix<T, M, N> prod;
    for (Int i = 0; i < M; i++)
        for (Int j = 0; j < N; j++)
            prod(i, j) = a(i) * b(j);
    return prod;
}

/// Convert DenseVector<DenseVector, M>, N> into a DenseMatrix<N, M>
///
/// @tparam T Data type
/// @tparam N Number of rows
/// @tparam M Number of columns
/// @param a Vector of vectors to be converted
/// @return DenseMatrix with values from `a`
template <typename T, Int N, Int M>
inline DenseMatrix<T, N, M>
mat_row(const DenseVector<DenseVector<T, M>, N> & a)
{
    DenseMatrix<T, N, M> res;
    for (Int i = 0; i < N; i++)
        for (Int j = 0; j < M; j++)
            res(i, j) = a(i)(j);
    return res;
}

/// Convert DenseVector<DenseVector, M>, N> into a DenseMatrix<M, N>
///
/// @tparam T Data type
/// @tparam N Number of rows in the input "matrix", but number of columns in the resulting matrix
/// @tparam M Number of columns in the input "matrix", but number of rows in the resulting matrix
/// @param a Input "matrix"
/// @return Transposed DenseMatrix with values from `a`
template <typename T, Int N, Int M>
inline DenseMatrix<T, M, N>
mat_col(const DenseVector<DenseVector<T, M>, N> & a)
{
    DenseMatrix<T, M, N> res;
    for (Int i = 0; i < N; i++)
        for (Int j = 0; j < M; j++)
            res(j, i) = a(i)(j);
    return res;
}

// Output

template <typename T, Int N>
std::ostream &
operator<<(std::ostream & os, const DenseVector<T, N> & obj)
{
    os << "(";
    for (Int i = 0; i < N; i++) {
        os << obj(i);
        if (i < N - 1)
            os << ", ";
    }
    os << ")";
    return os;
}

} // namespace godzilla
