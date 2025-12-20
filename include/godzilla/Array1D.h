// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "godzilla/Range.h"
#include "godzilla/Exception.h"
#include "godzilla/Assert.h"
#include "godzilla/Math.h"
#include <petscvec.h>

namespace godzilla {

template <typename T, Int N>
class DenseVector;
template <typename T, Int N, Int M>
class DenseMatrix;

template <typename T>
class Array1D {
private:
    struct ControlBlock {
        /// Reference count
        Int ref_count;
        /// Number of entries in the data block
        Int n;
    };

public:
    struct Iterator {
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using pointer = T *;
        using reference = T &;

        explicit Iterator(const Array1D * arr, Int idx) : arr(arr), idx(idx) {}

        value_type &
        operator*() const
        {
            return *(this->arr->data + this->idx);
        }

        /// Prefix increment
        Iterator &
        operator++()
        {
            ++this->idx;
            return *this;
        }

        /// Postfix increment
        Iterator
        operator++(int)
        {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool
        operator==(const Iterator & a, const Iterator & b)
        {
            return (a.arr == b.arr) && (a.idx == b.idx);
        };

        friend bool
        operator!=(const Iterator & a, const Iterator & b)
        {
            return (a.arr != b.arr) || (a.idx != b.idx);
        };

    private:
        /// Array to iterate over
        const Array1D * arr;
        /// Index pointing into the array
        Int idx;
    };

    struct ConstIterator {
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using pointer = const T *;
        using reference = const T &;

        explicit ConstIterator(const Array1D * arr, Int idx) : arr(arr), idx(idx) {}

        const value_type &
        operator*() const
        {
            return *(this->arr->data + this->idx);
        }

        /// Prefix increment
        ConstIterator &
        operator++()
        {
            ++this->idx;
            return *this;
        }

        /// Postfix increment
        ConstIterator
        operator++(int)
        {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool
        operator==(const ConstIterator & a, const ConstIterator & b)
        {
            return (a.arr == b.arr) && (a.idx == b.idx);
        };

        friend bool
        operator!=(const ConstIterator & a, const ConstIterator & b)
        {
            return (a.arr != b.arr) || (a.idx != b.idx);
        };

    private:
        /// Array to iterate over
        const Array1D * arr;
        /// Index pointing into the array
        Int idx;
    };

    /// Create an empty array
    Array1D() : ctrl(nullptr), first(0), data(nullptr) {}

    /// Create an array with specified number of entries
    ///
    /// @param size Number of entries in the array
    explicit Array1D(Int size) : ctrl(new ControlBlock { 1, size }), first(0), data(new T[size]) {}

    /// Create an array from a Range
    ///
    /// @param rng Indexing range
    explicit Array1D(const Range & rng) :
        ctrl(new ControlBlock { 1, rng.size() }),
        first(rng.first()),
        data(new T[rng.size()])
    {
    }

    ~Array1D() { release(); }

    // Copy constructor
    Array1D(const Array1D & other) : ctrl(other.ctrl), first(other.first), data(other.data)
    {
        if (this->ctrl)
            ++this->ctrl->ref_count;
    }

    // Copy assignment
    Array1D &
    operator=(const Array1D & other)
    {
        if (this != &other) {
            release();
            this->ctrl = other.ctrl;
            this->first = other.first;
            this->data = other.data;
            if (this->ctrl)
                ++this->ctrl->ref_count;
        }
        return *this;
    }

    // Move constructor
    Array1D(Array1D && other) noexcept :
        ctrl(std::exchange(other.ctrl, nullptr)),
        first(std::exchange(other.first, 0)),
        data(std::exchange(other.data, nullptr))
    {
    }

    // Move assignment
    Array1D &
    operator=(Array1D && other) noexcept
    {
        if (this != &other) {
            release();
            this->ctrl = std::exchange(other.ctrl, nullptr);
            this->first = std::exchange(other.first, 0);
            this->data = std::exchange(other.data, nullptr);
        }
        return *this;
    }

    explicit
    operator bool() const
    {
        return this->data != nullptr;
    }

    /// Get number of entries in the array
    ///
    /// @return Number of entries in the array
    Int
    size() const
    {
        return this->ctrl ? this->ctrl->n : 0;
    }

    /// Set all entries in the array to zero
    void
    zero()
    {
        assert_true(this->data != nullptr, "Internal storage is not allocated");
        for (Int i = 0; i < this->ctrl->n; ++i)
            this->data[i].zero();
    }

    /// Assign a value into all vector entries, i.e. `vec[i] = val`
    ///
    /// @param val Value to assign
    void
    set(const T & val)
    {
        assert_true(this->data != nullptr, "Internal storage is not allocated");
        for (Int i = 0; i < this->ctrl->n; ++i)
            this->data[i] = val;
    }

    // operators

    const T &
    operator()(Int i) const
    {
        return (*this)[i];
    }

    T &
    operator()(Int i)
    {
        return (*this)[i];
    }

    /// Get the entry at a specified location for reading
    ///
    /// @param i Index fo the entry
    /// @return Entry at the `ith` location
    const T &
    operator[](Int i) const
    {
        assert_true(this->data != nullptr, "Internal storage is not allocated");
        assert_true((i >= this->first) && (i < this->first + this->ctrl->n), "Index out of bounds");
        auto idx = i - this->first;
        return this->data[idx];
    }

    /// Get the entry at a specified location for writing
    ///
    /// @param i Index fo the entry
    /// @return Entry at the `ith` location
    T &
    operator[](Int i)
    {
        assert_true(this->data != nullptr, "Internal storage is not allocated");
        assert_true((i >= this->first) && (i < this->first + this->ctrl->n), "Index out of bounds");
        auto idx = i - this->first;
        return this->data[idx];
    }

    //

    // Do your best to avoid abusing this API
    T *
    get_data()
    {
        return this->data;
    }

    const T *
    get_data() const
    {
        return this->data;
    }

    Iterator
    begin()
    {
        return Iterator(this, 0);
    }

    Iterator
    end()
    {
        return Iterator(this, this->ctrl->n);
    }

    ConstIterator
    begin() const
    {
        return ConstIterator(this, 0);
    }

    ConstIterator
    end() const
    {
        return ConstIterator(this, this->ctrl->n);
    }

private:
    void
    release()
    {
        if (this->ctrl && --this->ctrl->ref_count == 0) {
            delete this->ctrl;
            delete[] this->data;
        }
    }

    /// Control block
    ControlBlock * ctrl;
    /// First index
    Int first;
    /// Array containing the values
    T * data;

    template <typename U>
    friend void pointwise_min(Array1D<U> & w, const Array1D<U> & x, const Array1D<U> & y);
    template <typename U>
    friend void pointwise_max(Array1D<U> & w, const Array1D<U> & x, const Array1D<U> & y);
    template <typename U>
    friend void pointwise_mult(Array1D<U> & w, const Array1D<U> & x, const Array1D<U> & y);
};

template <>
inline void
Array1D<Real>::zero()
{
    assert_true(this->data != nullptr, "Internal storage is not allocated");
    for (Int i = 0; i < this->ctrl->n; ++i)
        this->data[i] = 0.;
}

// Output

template <typename T>
std::ostream &
operator<<(std::ostream & os, const Array1D<T> & obj)
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

/// Get values from an array at specified locations
///
/// @tparam T C++ type
/// @tparam N number of values
/// @param data Array to get data from
/// @param idx Vector of indices to obtain data from
/// @return Vector of values from the array
template <typename T, Int N>
DenseVector<T, N>
get_values(const Array1D<T> & data, const DenseVector<Int, N> & idx)
{
    DenseVector<T, N> vals;
    for (Int i = 0; i < N; i++)
        vals(i) = data[idx(i)];
    return vals;
}

/// Get vector-valued entries from an array at specified locations
///
/// @tparam T C++ type
/// @tparam N number of values
/// @tparam M number of components in the vector
/// @param data Array to get data from
/// @param idx Vector of indices to obtain data from
/// @return Matrix of values from the array (rows are the vector-valued data)
template <typename T, Int N, Int M>
DenseMatrix<Real, N, M>
get_values(const Array1D<DenseVector<T, M>> & data, const DenseVector<Int, N> & idx)
{
    DenseMatrix<Real, N, M> vals;
    for (Int i = 0; i < N; ++i) {
        for (Int j = 0; j < M; ++j)
            vals(i, j) = data[idx(i)](j);
    }
    return vals;
}

/// Get values from an array at specified locations
///
/// @tparam T C++ type
/// @tparam N number of values
/// @param data Array to get data from
/// @param idx Vector of indices to obtain data from
/// @return Vector of values from the array
template <typename T, Int N>
DenseVector<T, N>
get_values(const Array1D<T> & data, const std::vector<Int> & idx)
{
    assert_true(N == idx.size(),
                "Size of `idx` argument does not match the size of the return value");
    DenseVector<T, N> vals;
    for (Int i = 0; i < N; ++i)
        vals(i) = data[idx[i]];
    return vals;
}

/// Set multiple values at specified indices
///
/// @tparam N Size of the array
/// @param idx Indices where the values are to be set
/// @param a Values to be set
template <typename T, Int N>
void
set_values(Array1D<T> & data, const DenseVector<Int, N> & idx, const DenseVector<T, N> & a)
{
    for (Int i = 0; i < N; ++i)
        data[idx(i)] = a(i);
}

/// Add values into the array at specified indices
///
/// @tparam T C++ type
/// @tparam N number of values
/// @param data akceli vector to add values to
/// @param idx Vector of indices into `data`
/// @param vals Vector of values to add to `data`
template <typename T, Int N>
void
add_values(Array1D<T> & data, const DenseVector<Int, N> & idx, const DenseVector<T, N> & vals)
{
    for (Int i = 0; i < N; ++i)
        data[idx(i)] += vals(i);
}

/// Assign values from `std::vector` into the `Array1D`
///
/// @param data Array1D to assign values into
/// @param vals Values to set into this vector
template <typename T>
void
assign(Array1D<T> & data, const std::vector<T> & vals)
{
    assert_true(data.size() == vals.size(),
                "Number of values to be assigned does not match the size the array");
    for (Int i = 0; i < data.size(); ++i)
        data[i] = vals[i];
}

/// Compute norm
///
/// @param vector Vector with values
/// @param type Norm type
/// @return Computed norm
template <FloatingPoint T>
T
norm(Array1D<T> & vector, NormType type)
{
    T norm = T(0);
    switch (type) {
    case NORM_1:
        for (const auto & val : vector)
            norm += std::abs(val);
        return norm;

    case NORM_2:
        for (const auto & val : vector)
            norm += val * val;
        return std::sqrt(norm);

    case NORM_INFINITY:
        for (const auto & val : vector)
            norm = std::max(norm, std::abs(val));
        return norm;

    default:
        throw NotImplementedException("`norm` not implemented for type {}", static_cast<int>(type));
    }
}

/// Compute pointwise minimum
///
/// @tparam T C++ type
/// @param w Resulting array
/// @param x First array
/// @param y Second array
template <typename T>
void
pointwise_min(Array1D<T> & w, const Array1D<T> & x, const Array1D<T> & y)
{
    assert_true(w.size() == x.size(), "The size of 'w' does not match the size 'x'");
    assert_true(w.size() == y.size(), "The size of 'w' does not match the size 'y'");
    for (Int i = 0; i < w.ctrl->n; ++i)
        w.data[i] = math::min(x.data[i], y.data[i]);
}

/// Compute pointwise maximum
///
/// @tparam T C++ type
/// @param w Resulting array
/// @param x First array
/// @param y Second array
template <typename T>
void
pointwise_max(Array1D<T> & w, const Array1D<T> & x, const Array1D<T> & y)
{
    assert_true(w.size() == x.size(), "The size of 'w' does not match the size 'x'");
    assert_true(w.size() == y.size(), "The size of 'w' does not match the size 'y'");
    for (Int i = 0; i < w.ctrl->n; ++i)
        w.data[i] = math::max(x.data[i], y.data[i]);
}

/// Compute pointwise multiplication of elements
///
/// @tparam T C++ type
/// @param w Resulting array
/// @param x First array
/// @param y Second array
template <typename T>
void
pointwise_mult(Array1D<T> & w, const Array1D<T> & x, const Array1D<T> & y)
{
    assert_true(w.size() == x.size(), "The size of 'w' does not match the size 'x'");
    assert_true(w.size() == y.size(), "The size of 'w' does not match the size 'y'");
    for (Int i = 0; i < w.ctrl->n; ++i)
        w.data[i] = x.data[i] * y.data[i];
}

} // namespace godzilla
