// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "godzilla/Error.h"
#include <cassert>

namespace godzilla {

template <typename T, Int ROWS, Int COLS>
class DenseMatrix;
template <typename T, Int ROWS>
class DenseMatrixSymm;

/// Dense vector with `N` entries
///
/// @tparam T Data type of matrix entries
/// @tparam N Number of entries
template <typename T, Int N>
class DenseVector {
public:
    /// Create empty vector
    DenseVector() {}

    /// Create vector from a std::vector
    ///
    /// @param a Values to initialize the vector with
    explicit DenseVector(const std::vector<T> & a)
    {
        for (Int i = 0; i < N; i++)
            this->values[i] = a[i];
    }

    /// Get an entry at location (i) for reading
    ///
    /// @param i Index
    /// @return Entry at location (i)
    const T &
    get(Int i) const
    {
        assert((i >= 0) && (i < N));
        return this->values[i];
    }

    /// Get an entry at location (i) for writing
    ///
    /// @param i Index
    /// @return Entry at location (i)
    T &
    set(Int i)
    {
        assert((i >= 0) && (i < N));
        return this->values[i];
    }

    /// Multiply all entries by a scalar `alpha`, i.e. vec[i] = alpha * vec[i]
    ///
    /// @param alpha Scalar value to multiply entries with
    void
    scale(Real alpha)
    {
        for (Int i = 0; i < N; i++)
            this->values[i] *= alpha;
    }

    /// Set all vector elements to zero, i.e. vec[i] = 0.
    void
    zero()
    {
        zero_impl(std::is_fundamental<T>());
    }

    /// Set `alpha` into all vector elements, i.e. vec[i] = alpha
    ///
    /// @param alpha Constant to set into vector elements
    void
    set_values(const T & alpha)
    {
        for (Int i = 0; i < N; i++)
            this->values[i] = alpha;
    }

    /// Add `a` to this vector, i.e. vec[i] += a[i]
    ///
    /// @param a Vector to add
    void
    add(const DenseVector<T, N> & a)
    {
        for (Int i = 0; i < N; i++)
            this->values[i] += a(i);
    }

    /// Add `a` to each element of this vector, i.e. vec[i] += a
    ///
    /// @param a Value to add
    void
    add(T a)
    {
        for (Int i = 0; i < N; i++)
            this->values[i] += a;
    }

    /// Subtract `a` from this vector, i.e. vec[i] -= a[i]
    ///
    /// @param a Vector to subtract
    void
    subtract(const DenseVector<T, N> & a)
    {
        for (Int i = 0; i < N; i++)
            this->values[i] -= a(i);
    }

    /// Normalize this vector
    void
    normalize()
    {
        T mag = magnitude();
        if (mag > 0) {
            for (Int i = 0; i < N; i++)
                this->values[i] /= mag;
        }
    }

    /// Compute average from vector entries
    ///
    /// @return Average of vector entries
    Real
    avg() const
    {
        Real res = 0.;
        for (Int i = 0; i < N; i++)
            res += this->values[i];
        return res / N;
    }

    /// Compute dot product, i.e. \Sum_i vec[i] * a[i]
    ///
    /// @param a Dotted vector
    /// @return Dot product
    T
    dot(const DenseVector<T, N> & a) const
    {
        T dot = 0.;
        for (Int i = 0; i < N; i++)
            dot += this->values[i] * a(i);
        return dot;
    }

    DenseVector<Real, N>
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
    T
    sum() const
    {
        T sum = 0.;
        for (Int i = 0; i < N; i++)
            sum += this->values[i];
        return sum;
    }

    /// Compute vector magnitude, i.e. sqrt(\Sum_i vec[i]^2)
    ///
    /// @return Vector magnitude
    T
    magnitude() const
    {
        T sum = 0.;
        for (Int i = 0; i < N; i++)
            sum += this->values[i] * this->values[i];
        return std::sqrt(sum);
    }

    /// Point-wise multiplication of this vector with another one
    ///
    /// @param b Second vector
    /// @return Vector with entries this[i]*b[i]
    DenseVector<T, N>
    pointwise_mult(const DenseVector<T, N> & b) const
    {
        DenseVector<T, N> res;
        for (Int i = 0; i < N; i++)
            res(i) = get(i) * b(i);
        return res;
    }

    /// Point-wise division of this vector with another one
    ///
    /// @param b Second vector
    /// @return Vector with entries this[i]/b[i]
    DenseVector<T, N>
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
    T
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
    T
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
    DenseVector<T, M>
    operator*(const DenseMatrix<T, N, M> & a) const
    {
        DenseVector<T, M> res;
        for (Int j = 0; j < M; j++) {
            T prod = 0;
            for (Int i = 0; i < N; i++)
                prod += get(i) * a(i, j);
            res(j) = prod;
        }
        return res;
    }

    DenseVector<T, N>
    operator*(const DenseMatrixSymm<T, N> & a) const
    {
        DenseVector<T, N> res;
        for (Int j = 0; j < N; j++) {
            T prod = 0;
            for (Int i = 0; i < N; i++)
                prod += get(i) * a(i, j);
            res(j) = prod;
        }
        return res;
    }

    template <Int M>
    DenseVector<T, M>
    operator*(const DenseVector<DenseVector<T, N>, M> & a) const
    {
        DenseVector<T, M> res;
        for (Int j = 0; j < M; j++)
            res(j) = *this * a(j);
        return res;
    }

    T
    operator*(const DenseVector<T, N> & a) const
    {
        return this->dot(a);
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

protected:
    void
    zero_impl(std::true_type)
    {
        set_values(0);
    }

    void
    zero_impl(std::false_type)
    {
        for (Int i = 0; i < N; i++)
            this->values[i].zero();
    }

private:
    T values[N];
};

/// Compute dot product of 2 vectors
///
/// @tparam T Data type
/// @tparam N Size of the vector
/// @param a First vector
/// @param b Second vector
/// @return Dot product
template <typename T, Int N>
inline T
dot(const DenseVector<T, N> & a, const DenseVector<T, N> & b)
{
    T dot = 0.;
    for (Int i = 0; i < N; i++)
        dot += a(i) * b(i);
    return dot;
}

/// Pointwise multiplication of 2 vectors
///
/// @tparam T Data type
/// @tparam N Size of the vector
/// @param a First vector
/// @param b Second vector
/// @return Vector with vec[i] = a[i] * b[i]
template <typename T, Int N>
inline DenseVector<T, N>
pointwise_mult(const DenseVector<T, N> & a, const DenseVector<T, N> & b)
{
    DenseVector<T, N> res;
    for (Int i = 0; i < N; i++)
        res(i) = a(i) * b(i);
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

template <typename T, Int N>
inline DenseMatrix<T, N, 1>
mat_row(const DenseVector<T, N> & a)
{
    DenseMatrix<T, N, 1> res;
    for (Int i = 0; i < N; i++)
        res(i, 0) = a(i);
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

/// Convert a DenseVector<T, N> into a column matrix
///
/// @tparam T Data type
/// @tparam N Number of columns
/// @param a Input vector
/// @return Column matrix with values of `a`
template <typename T, Int N>
inline DenseMatrix<T, 1, N>
mat_col(const DenseVector<T, N> & a)
{
    DenseMatrix<T, 1, N> res;
    for (Int i = 0; i < N; i++)
        res(0, i) = a(i);
    return res;
}

/// Transpose DenseVector<DenseVector<T>>
///
/// @tparam T Data type
/// @tparam N Number of rows in the input "matrix", but number of columns in the resulting matrix
/// @tparam M Number of columns in the input "matrix", but number of rows in the resulting matrix
/// @param a Input "matrix"
/// @return Transposed version of DenseVector<DenseVector<T>> with values from `a`
template <typename T, Int N, Int M>
inline DenseVector<DenseVector<T, N>, M>
transpose(const DenseVector<DenseVector<T, M>, N> & a)
{
    DenseVector<DenseVector<T, N>, M> res;
    for (Int i = 0; i < N; i++)
        for (Int j = 0; j < M; j++)
            res(j)(i) = a(i)(j);
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
