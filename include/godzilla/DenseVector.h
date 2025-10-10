// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Exception.h"
#include "godzilla/Types.h"
#include "godzilla/Math.h"
#include "godzilla/DenseMatrix.h"
#include "godzilla/Utils.h"
#include "mpicpp-lite/mpicpp-lite.h"
#include <cassert>
#include <initializer_list>
#include <type_traits>

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
    using value_type = T;

    /// Create empty vector
    DenseVector() = default;

    /// Create vector from a std::vector
    ///
    /// @param a Values to initialize the vector with
    explicit DenseVector(const std::vector<T> & a)
    {
        for (Int i = 0; i < N; ++i)
            set(i) = a[i];
    }

    DenseVector(const DenseMatrix<T, N, 1> & a)
    {
        for (Int i = 0; i < N; ++i)
            set(i) = a(i, 0);
    }

    template <Int M = N>
        requires(M > 0)
    explicit DenseVector(std::initializer_list<T> init)
    {
        for (const auto & [i, v] : enumerate(init))
            set(i) = v;
    }

    template <Int M = N>
        requires(M > 0)
    explicit DenseVector(const std::array<T, M> & init)
    {
        for (Int i = 0; i < N; ++i)
            set(i) = init(i);
    }

    /// Get an entry at location (i) for reading
    ///
    /// @param i Index
    /// @return Entry at location (i)
    const T &
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
        for (Int i = 0; i < N; ++i)
            set(i) += a(i);
    }

    /// Add `a` to each element of this vector, i.e. vec[i] += a
    ///
    /// @param a Value to add
    void
    add(T a)
    {
        for (Int i = 0; i < N; ++i)
            set(i) += a;
    }

    /// Subtract `a` from this vector, i.e. vec[i] -= a[i]
    ///
    /// @param a Vector to subtract
    void
    subtract(const DenseVector<T, N> & a)
    {
        for (Int i = 0; i < N; ++i)
            set(i) -= a(i);
    }

    /// Normalize this vector
    void
    normalize()
    {
        T mag = magnitude();
        if (mag > 0) {
            for (Int i = 0; i < N; ++i)
                set(i) /= mag;
        }
    }

    /// Compute normalized version of this vector
    ///
    /// @return This vector normalized
    DenseVector<T, N>
    normalized() const
    {
        DenseVector<T, N> res;
        T mag = magnitude();
        if (mag > 0) {
            for (Int i = 0; i < N; ++i)
                res(i) = get(i) / mag;
        }
        else
            res.zero();
        return res;
    }

    /// Compute average from vector entries
    ///
    /// @return Average of vector entries
    Real
    avg() const
    {
        Real res = 0.;
        for (Int i = 0; i < N; ++i)
            res += get(i);
        return res / N;
    }

    /// Sum all vector elements, i.e \Sum_i vec[i]
    ///
    /// @return Sum of all elements
    T
    sum() const
    {
        T sum = 0.;
        for (Int i = 0; i < N; ++i)
            sum += get(i);
        return sum;
    }

    /// Compute vector magnitude, i.e. sqrt(\Sum_i vec[i]^2)
    ///
    /// @return Vector magnitude
    T
    magnitude() const
    {
        T sum = 0.;
        for (Int i = 0; i < N; ++i)
            sum += get(i) * get(i);
        return std::sqrt(sum);
    }

    /// Find the minimum value of the elements
    ///
    /// @return The minimum value of the elements
    T
    min() const
    {
        T res = std::numeric_limits<T>::max();
        for (Int i = 0; i < N; ++i)
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
        for (Int i = 0; i < N; ++i)
            if (get(i) > res)
                res = get(i);
        return res;
    }

    /// Replaces every element in a vector with its absolute value
    void
    abs()
    {
        for (Int i = 0; i < N; ++i)
            set(i) = math::abs(get(i));
    }

    // Operators

    template <Int M = N>
        requires(M > 0)
    DenseVector<T, N>
    operator=(std::array<T, N> vals)
    {
        for (Int i = 0; i < N; ++i)
            set(i) = vals[i];
        return *this;
    }

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
        for (Int i = 0; i < N; ++i)
            res(i) = get(i) + a(i);
        return res;
    }

    DenseVector<T, N>
    operator-() const
    {
        DenseVector<T, N> res;
        for (Int i = 0; i < N; ++i)
            res(i) = -get(i);
        return res;
    }

    DenseVector<T, N>
    operator-(const DenseVector<T, N> & a) const
    {
        DenseVector<T, N> res;
        for (Int i = 0; i < N; ++i)
            res(i) = get(i) - a(i);
        return res;
    }

    template <Int M>
    DenseMatrix<T, N, M>
    operator*(const DenseMatrix<T, 1, M> & a) const
    {
        DenseMatrix<T, N, M> res;
        for (Int i = 0; i < M; ++i)
            for (Int j = 0; j < N; ++j)
                res(j, i) = get(j) * a(0, i);
        return res;
    }

    DenseVector<T, N>
    operator*(Real alpha) const
    {
        DenseVector<T, N> res;
        for (Int i = 0; i < N; ++i)
            res(i) = alpha * get(i);
        return res;
    }

    DenseVector<T, N> &
    operator+=(const DenseVector<T, N> & a)
    {
        for (Int i = 0; i < N; ++i)
            set(i) += a(i);
        return *this;
    }

    DenseVector<T, N> &
    operator+=(const T & a)
    {
        for (Int i = 0; i < N; ++i)
            set(i) += a;
        return *this;
    }

    DenseVector<T, N> &
    operator-=(const DenseVector<T, N> & a)
    {
        for (Int i = 0; i < N; ++i)
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
    for (Int i = 0; i < N; ++i)
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
    for (Int i = 0; i < N; ++i)
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
    for (Int i = 0; i < N; ++i)
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
    for (Int i = 0; i < N; ++i)
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
    for (Int i = 0; i < N; ++i)
        res(0, i) = a(0, i) * b(0, i);
    return res;
}

/// Pointwise division of 2 column-vectors
///
/// @tparam T Data type
/// @tparam N Size of the vector
/// @param a First column-vector
/// @param b Second column-vector
/// @return Column-vector with vec[i] = a[i] / b[i]
template <typename T, Int N>
inline DenseMatrix<T, N, 1>
pointwise_div(const DenseMatrix<T, N, 1> & a, const DenseMatrix<T, N, 1> & b)
{
    DenseMatrix<T, N, 1> res;
    for (Int i = 0; i < N; ++i)
        res(i, 0) = a(i, 0) / b(i, 0);
    return res;
}

/// Pointwise division of 2 row-vectors
///
/// @tparam T Data type
/// @tparam N Size of the vector
/// @param a First row-vector
/// @param b Second row-vector
/// @return Row-vector with vec[i] = a[i] / b[i]
template <typename T, Int N>
inline DenseMatrix<T, 1, N>
pointwise_div(const DenseMatrix<T, 1, N> & a, const DenseMatrix<T, 1, N> & b)
{
    DenseMatrix<T, 1, N> res;
    for (Int i = 0; i < N; ++i)
        res(0, i) = a(0, i) / b(0, i);
    return res;
}

template <typename T, Int N>
inline DenseVector<T, N>
operator*(Real alpha, const DenseVector<T, N> & a)
{
    DenseVector<T, N> res;
    for (Int i = 0; i < N; ++i)
        res(i) = alpha * a(i);
    return res;
}

// Cross product

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
    for (Int i = 0; i < N; ++i)
        for (Int j = 0; j < M; ++j)
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
    for (Int i = 0; i < N; ++i)
        for (Int j = 0; j < M; ++j)
            res(j, i) = a(i)(j);
    return res;
}

/// Compute magnitude of a vector
///
/// @tparam T Data type
/// @tparam N Number of elements in the vector
/// @param vec Vector
/// @return Magnitude of the vector
template <typename T, Int N>
T
magnitude(const DenseVector<T, N> & vec)
{
    T sum = 0.;
    for (Int i = 0; i < N; ++i)
        sum += vec(i) * vec(i);
    return std::sqrt(sum);
}

// Output

template <typename T, Int N>
std::ostream &
operator<<(std::ostream & os, const DenseVector<T, N> & obj)
{
    os << "(";
    for (Int i = 0; i < N; ++i) {
        os << obj(i);
        if (i < N - 1)
            os << ", ";
    }
    os << ")";
    return os;
}

// Dynamic dense vector

template <typename T>
using DynDenseVector = DenseVector<T, -1>;

template <typename T>
class DenseVector<T, -1> : public DynDenseMatrix<T> {
public:
    using value_type = T;

    DenseVector() : DynDenseMatrix<T>() {}

    explicit DenseVector(Int rows) : DynDenseMatrix<T>(rows, 1) {}

    DenseVector(Int rows, const T & val) : DynDenseMatrix<T>(rows, 1, val) {}

    DenseVector(const DenseVector & other) : DynDenseMatrix<T>(other) {}

    /// Get an entry at location (i) for reading
    ///
    /// @param i Index
    /// @return Entry at location (i)
    const T &
    get(Int i) const
    {
        if ((i >= 0) && (i < size()))
            return DynDenseMatrix<T>::get(i, 0);
        else
            throw Exception("Index ({}) is out of vector dimension ({})", i, size());
    }

    /// Get entry at specified location for writing
    ///
    /// @param i Row number
    /// @return Entry at the specified location
    T &
    set(Int i)
    {
        if ((i >= 0) && (i < size()))
            return DynDenseMatrix<T>::set(i, 0);
        else
            throw Exception("Index ({}) is out of vector dimension ({})", i, size());
    }

    /// Resize the vector
    ///
    /// @param n New size of the vector
    void
    resize(Int n)
    {
        DynDenseMatrix<T>::resize(n, 1);
    }

    /// Multiply all entries by a scalar value
    ///
    /// @param alpha Value to multiple the vector entries with
    void
    scale(Real alpha)
    {
        DynDenseMatrix<T>::scale(alpha);
    }

    /// Add `a` to this vector, i.e. vec[i] += a[i]
    ///
    /// @param a Vector to add
    void
    add(const DynDenseVector<T> & a)
    {
        if (a.size() == size()) {
            for (Int i = 0; i < size(); ++i)
                set(i) += a(i);
        }
        else
            throw Exception("Size of the operand ({}) must match the vector size ({})",
                            a.size(),
                            size());
    }

    /// Add `a` to each element of this vector, i.e. vec[i] += a
    ///
    /// @param a Value to add
    void
    add(T a)
    {
        for (Int i = 0; i < size(); ++i)
            set(i) += a;
    }

    /// Subtract `a` from this vector, i.e. vec[i] -= a[i]
    ///
    /// @param a Vector to subtract
    void
    subtract(const DynDenseVector<T> & a)
    {
        if (a.size() == size()) {
            for (Int i = 0; i < size(); ++i)
                set(i) -= a(i);
        }
        else
            throw Exception("Size of the operand ({}) must match the vector size ({})",
                            a.size(),
                            size());
    }

    /// Normalize this vector
    void
    normalize()
    {
        T mag = magnitude();
        if (mag > 0) {
            for (Int i = 0; i < size(); ++i)
                set(i) /= mag;
        }
    }

    /// Compute normalized version of this vector
    ///
    /// @return This vector normalized
    DenseVector<T, -1>
    normalized() const
    {
        DenseVector<T, -1> res(size());
        T mag = magnitude();
        if (mag > 0) {
            for (Int i = 0; i < size(); ++i)
                res(i) = get(i) / mag;
        }
        else
            res.zero();
        return res;
    }

    /// Compute average from vector entries
    ///
    /// @return Average of vector entries
    Real
    avg() const
    {
        Real res = 0.;
        for (Int i = 0; i < size(); ++i)
            res += get(i);
        return res / size();
    }

    /// Sum all vector elements, i.e \Sum_i vec[i]
    ///
    /// @return Sum of all elements
    T
    sum() const
    {
        T sum = 0.;
        for (Int i = 0; i < size(); ++i)
            sum += get(i);
        return sum;
    }

    /// Compute vector magnitude, i.e. sqrt(\Sum_i vec[i]^2)
    ///
    /// @return Vector magnitude
    T
    magnitude() const
    {
        T sum = 0.;
        for (Int i = 0; i < size(); ++i)
            sum += get(i) * get(i);
        return std::sqrt(sum);
    }

    /// Find the minimum value of the elements
    ///
    /// @return The minimum value of the elements
    T
    min() const
    {
        T res = std::numeric_limits<T>::max();
        for (Int i = 0; i < size(); ++i)
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
        for (Int i = 0; i < size(); ++i)
            if (get(i) > res)
                res = get(i);
        return res;
    }

    /// Replaces every element in a vector with its absolute value
    void
    abs()
    {
        for (Int i = 0; i < size(); ++i)
            set(i) = math::abs(get(i));
    }

    /// Set all vector entries to zero
    void
    zero()
    {
        DynDenseMatrix<T>::zero();
    }

    /// Set `alpha` into all entries, i.e. vec[i] = alpha
    ///
    /// @param alpha Value to set into vector entries
    void
    set_values(const T & alpha)
    {
        DynDenseMatrix<T>::set_values(alpha);
    }

    void
    set_values(const std::vector<T> & vals)
    {
        DynDenseMatrix<T>::set_col(0, vals);
    }

    /// Get the size of the vector
    ///
    /// @return Size of the vector
    Int
    size() const
    {
        return this->get_num_rows();
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

    DynDenseVector<T>
    operator+(const DynDenseVector<T> & a) const
    {
        if (size() == a.size()) {
            DynDenseVector<T> res(a.size());
            for (Int i = 0; i < size(); ++i)
                res(i) = get(i) + a(i);
            return res;
        }
        else
            throw Exception("Size of the operand ({}) must match the vector size ({})",
                            a.size(),
                            size());
    }

    DynDenseVector<T>
    operator-() const
    {
        DynDenseVector<T> res(size());
        for (Int i = 0; i < size(); ++i)
            res(i) = -get(i);
        return res;
    }

    DynDenseVector<T>
    operator-(const DynDenseVector<T> & a) const
    {
        if (size() == a.size()) {
            DynDenseVector<T> res(size());
            for (Int i = 0; i < size(); ++i)
                res(i) = get(i) - a(i);
            return res;
        }
        else
            throw Exception("Size of the operand ({}) must match the vector size ({})",
                            a.size(),
                            size());
    }

    DynDenseVector<T>
    operator*(Real alpha) const
    {
        DynDenseVector<T> res(size());
        for (Int i = 0; i < size(); ++i)
            res(i) = alpha * get(i);
        return res;
    }

    DynDenseVector<T> &
    operator+=(const DynDenseVector<T> & a)
    {
        if (size() == a.size()) {
            for (Int i = 0; i < size(); ++i)
                set(i) += a(i);
            return *this;
        }
        throw Exception("Size of the operand ({}) must match the vector size ({})",
                        a.size(),
                        size());
    }

    DynDenseVector<T> &
    operator+=(const T & a)
    {
        for (Int i = 0; i < size(); ++i)
            set(i) += a;
        return *this;
    }

    DynDenseVector<T> &
    operator-=(const DynDenseVector<T> & a)
    {
        if (size() == a.size()) {
            for (Int i = 0; i < size(); ++i)
                set(i) -= a(i);
            return *this;
        }
        throw Exception("Size of the operand ({}) must match the vector size ({})",
                        a.size(),
                        size());
    }
};

/// Compute dot product of 2 column-vectors
///
/// @tparam T Data type
/// @param a First column-vector
/// @param b Second column-vector
/// @return Dot product
template <typename T>
inline T
dot(const DynDenseVector<T> & a, const DynDenseVector<T> & b)
{
    if (a.size() == b.size()) {
        T dot = 0.;
        for (Int i = 0; i < a.size(); ++i)
            dot += a(i) * b(i);
        return dot;
    }
    else
        throw Exception("Size of vector a ({}) must match size of vector b ({})",
                        a.size(),
                        b.size());
}

/// Pointwise multiplication of 2 column-vectors
///
/// @tparam T Data type
/// @param a First column-vector
/// @param b Second column-vector
/// @return column-vector with vec[i] = a[i] * b[i]
template <typename T>
inline DynDenseVector<T>
pointwise_mult(const DynDenseVector<T> & a, const DynDenseVector<T> & b)
{
    if (a.size() == b.size()) {
        DynDenseVector<T> res(a.size());
        for (Int i = 0; i < a.size(); ++i)
            res(i) = a(i) * b(i);
        return res;
    }
    else
        throw Exception("Size of vector a ({}) must match size of vector b ({})",
                        a.size(),
                        b.size());
}

/// Pointwise division of 2 column-vectors
///
/// @tparam T Data type
/// @param a First column-vector
/// @param b Second column-vector
/// @return Column-vector with vec[i] = a[i] / b[i]
template <typename T>
inline DynDenseVector<T>
pointwise_div(const DynDenseVector<T> & a, const DynDenseVector<T> & b)
{
    if (a.size() == b.size()) {
        DynDenseVector<T> res(a.size());
        for (Int i = 0; i < a.size(); ++i)
            res(i) = a(i) / b(i);
        return res;
    }
    else
        throw Exception("Size of vector a ({}) must match size of vector b ({})",
                        a.size(),
                        b.size());
}

template <typename T>
inline DynDenseVector<T>
operator*(Real alpha, const DynDenseVector<T> & a)
{
    DynDenseVector<T> res(a.size());
    for (Int i = 0; i < a.size(); ++i)
        res(i) = alpha * a(i);
    return res;
}

/// Compute cross product from 2 vectors
///
/// @param a First vector
/// @param b Second vector
/// @return Resulting vector \f$ a x b\f$
inline DynDenseVector<Real>
cross_product(const DynDenseVector<Real> & a, const DynDenseVector<Real> & b)
{
    if ((a.size() == 3) && (b.size() == 3)) {
        DynDenseVector<Real> res(3);
        res(0) = a(1) * b(2) - a(2) * b(1);
        res(1) = -(a(0) * b(2) - a(2) * b(0));
        res(2) = a(0) * b(1) - a(1) * b(0);
        return res;
    }
    else
        throw Exception("Cross-product id defined only for vectors of dimension 3");
}

template <typename T>
std::ostream &
operator<<(std::ostream & os, const DynDenseVector<T> & obj)
{
    os << "(";
    for (Int i = 0; i < obj.size(); ++i) {
        os << obj(i);
        if (i < obj.size() - 1)
            os << ", ";
    }
    os << ")";
    return os;
}

} // namespace godzilla

namespace mpicpp_lite {

template <>
inline MPI_Datatype
mpi_datatype<godzilla::DenseVector<godzilla::Real, 1>>()
{
    return mpi_datatype<godzilla::Real>();
}

template <>
inline MPI_Datatype
mpi_datatype<godzilla::DenseVector<godzilla::Int, 1>>()
{
    return mpi_datatype<godzilla::Int>();
}

template <typename T, godzilla::Int D>
struct DatatypeTraits<godzilla::DenseVector<T, D>> {
    static MPI_Datatype
    get()
    {
        // D=1 hoses up MPI internally, so we need to prevent the code from compiling.
        // for D=1, use `mpi_datatype` instead.
        static_assert(D > 1);
        return type_contiguous(D, mpi_datatype<T>());
    }
};

namespace op {

template <typename T, godzilla::Int D>
struct sum<godzilla::DenseVector<T, D>> :
    public UserOp<sum<godzilla::DenseVector<T, D>>, godzilla::DenseVector<T, D>> {
public:
    godzilla::DenseVector<T, D>
    operator()(const godzilla::DenseVector<T, D> & x, const godzilla::DenseVector<T, D> & y) const
    {
        return x + y;
    }
};

template <typename T, godzilla::Int D>
struct prod<godzilla::DenseVector<T, D>> :
    public UserOp<prod<godzilla::DenseVector<T, D>>, godzilla::DenseVector<T, D>> {
public:
    godzilla::DenseVector<T, D>
    operator()(const godzilla::DenseVector<T, D> & x, const godzilla::DenseVector<T, D> & y) const
    {
        throw godzilla::Exception("Unable to use DenseVector with mpi::op::prod");
    }
};

template <typename T, godzilla::Int D>
struct max<godzilla::DenseVector<T, D>> :
    public UserOp<max<godzilla::DenseVector<T, D>>, godzilla::DenseVector<T, D>> {
public:
    godzilla::DenseVector<T, D>
    operator()(const godzilla::DenseVector<T, D> & x, const godzilla::DenseVector<T, D> & y) const
    {
        throw godzilla::Exception("Unable to use DenseVector with mpi::op::max");
    }
};

template <typename T, godzilla::Int D>
struct min<godzilla::DenseVector<T, D>> :
    public UserOp<min<godzilla::DenseVector<T, D>>, godzilla::DenseVector<T, D>> {
public:
    godzilla::DenseVector<T, D>
    operator()(const godzilla::DenseVector<T, D> & x, const godzilla::DenseVector<T, D> & y) const
    {
        throw godzilla::Exception("Unable to use DenseVector with mpi::op::min");
    }
};

} // namespace op
} // namespace mpicpp_lite
