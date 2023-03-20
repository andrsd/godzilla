#pragma once

#include "Types.h"
#include "Error.h"
#include <cassert>

namespace godzilla {

template <typename T, Int ROWS, Int COLS>
class DenseMatrix;

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
            this->data[i] = a[i];
    }

    /// Get an entry at location (i) for reading
    ///
    /// @param i Index
    /// @return Entry at location (i)
    const T &
    get(Int i) const
    {
        assert((i >= 0) && (i < N));
        return this->data[i];
    }

    /// Get an entry at location (i) for writing
    ///
    /// @param i Index
    /// @return Entry at location (i)
    T &
    set(Int i)
    {
        assert((i >= 0) && (i < N));
        return this->data[i];
    }

    /// Multiply all entries by a scalar `alpha`, i.e. vec[i] = alpha * vec[i]
    ///
    /// @param alpha Scalar value to multiply entries with
    void
    scale(Real alpha)
    {
        for (Int i = 0; i < N; i++)
            this->data[i] *= alpha;
    }

    /// Set all vector elements to zero, i.e. vec[i] = 0.
    void
    zero()
    {
        for (Int i = 0; i < N; i++)
            this->data[i].zero();
    }

    /// Set `alpha` into all vector elements, i.e. vec[i] = alpha
    ///
    /// @param alpha Constant to set into vector elements
    void
    set_values(const T & alpha)
    {
        for (Int i = 0; i < N; i++)
            this->data[i] = alpha;
    }

    /// Add `a` to this vector, i.e. vec[i] += a[i]
    ///
    /// @param a Vector to add
    void
    add(const DenseVector<T, N> & a)
    {
        for (Int i = 0; i < N; i++)
            this->data[i] += a(i);
    }

    /// Compute average from vector entries
    ///
    /// @return Average of vector entries
    Real
    avg() const
    {
        Real res = 0.;
        for (Int i = 0; i < N; i++)
            res += this->data[i];
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
            dot += this->data[i] * a(i);
        return dot;
    }

    DenseVector<Real, N>
    cross(const DenseVector<Real, N> &) const
    {
        error("Cross product in %d dimensions is not unique.", N);
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
            sum += this->data[i];
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
            sum += this->data[i] * this->data[i];
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
    T data[N];
};

/// Compute dot product of 2 vectors
///
/// @tparam T Data type
/// @tparam DIM Dimension of the vector
/// @param a First vector
/// @param b Second vector
/// @return Dot product
template <typename T, Int DIM>
inline T
dot(const DenseVector<T, DIM> & a, const DenseVector<T, DIM> & b)
{
    T dot = 0.;
    for (Int i = 0; i < DIM; i++)
        dot += a(i) * b(i);
    return dot;
}

/// Pointwise multiplication of 2 vectors
///
/// @tparam T Data type
/// @tparam DIM Dimension of the vectors
/// @param a First vector
/// @param b Second vector
/// @return Vector with vec[i] = a[i] * b[i]
template <typename T, Int DIM>
inline DenseVector<T, DIM>
pointwise_mult(const DenseVector<T, DIM> & a, const DenseVector<T, DIM> & b)
{
    DenseVector<T, DIM> res;
    for (Int i = 0; i < DIM; i++)
        res(i) = a(i) * b(i);
    return res;
}

/// Pointwise division of 2 vectors
///
/// @tparam T Data type
/// @tparam DIM Dimension of the vectors
/// @param a First vector
/// @param b Second vector
/// @return Vector with vec[i] = a[i] / b[i]
template <typename T, Int DIM>
inline DenseVector<T, DIM>
pointwise_div(const DenseVector<T, DIM> & a, const DenseVector<T, DIM> & b)
{
    DenseVector<T, DIM> res;
    for (Int i = 0; i < DIM; i++)
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

/// Convert DenseVector<DenseVector, M>, N> into a DenseMatrix<N, M>
///
/// @tparam T Data type
/// @tparam N Number of rows
/// @tparam M Number of columns
/// @param a Vector of vectors to be converted
/// @return DenseMatrix with values from `a`
template <typename T, Int N, Int M>
inline DenseMatrix<T, N, M>
mat(const DenseVector<DenseVector<T, M>, N> & a)
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
mat_transpose(const DenseVector<DenseVector<T, M>, N> & a)
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
