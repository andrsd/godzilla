// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "godzilla/Error.h"
#include "godzilla/Range.h"
#include <cassert>

namespace godzilla {

template <typename T, Int N>
class DenseVector;
template <typename T, Int N, Int M>
class DenseMatrix;

template <typename T>
class Array1D {
public:
    struct Iterator {
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using pointer = T *;
        using reference = T &;

        explicit Iterator(const Array1D & arr, Int idx) : arr(arr), idx(idx) {}

        const value_type &
        operator*() const
        {
            return *(this->arr.data + this->idx);
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
            return (&a.arr == &b.arr) && (a.idx == b.idx);
        };

        friend bool
        operator!=(const Iterator & a, const Iterator & b)
        {
            return (&a.arr != &b.arr) || (a.idx != b.idx);
        };

    private:
        /// IndexSet to iterate over
        const Array1D & arr;
        /// Index pointing to the `is`
        Int idx;
    };

    /// Create an empty array
    Array1D() : n(0), data(nullptr) {}

    /// Create an array with specified number of entries
    ///
    /// @param size Number of entries in the array
    explicit Array1D(Int size) : n(size), data(new T[size]), range(0, size) {}

    /// Allocate memory for the array with specified number of entries
    ///
    /// @param size Number of entries in the array
    void
    create(Int size)
    {
        this->range = Range(0, size);
        this->n = size;
        this->data = new T[size];
    }

    void
    create(const Range & rng)
    {
        this->range = rng;
        auto size = rng.last() - rng.first();
        this->n = size;
        this->data = new T[size];
    }

    /// Get number of entries in the array
    ///
    /// @return Number of entries in the array
    Int
    get_size() const
    {
        return this->n;
    }

    /// Get the entry at a specified location for reading
    ///
    /// @param i Index fo the entry
    /// @return Entry at the `ith` location
    const T &
    get(Int i) const
    {
        assert(this->data != nullptr);
        assert((i >= this->range.first()) && (i < this->range.last()));
        auto idx = i - this->range.first();
        return this->data[idx];
    }

    /// Get the entry at a specified location for writing
    ///
    /// @param i Index fo the entry
    /// @return Entry at the `ith` location
    T &
    set(Int i)
    {
        assert(this->data != nullptr);
        assert((i >= this->range.first()) && (i < this->range.last()));
        auto idx = i - this->range.first();
        return this->data[idx];
    }

    /// Set all entries in the array to zero
    void
    zero()
    {
        assert(this->data != nullptr);
        for (Int i = 0; i < this->n; ++i)
            this->data[i].zero();
    }

    /// Free memory allocated by this array
    void
    destroy()
    {
        delete[] this->data;
        this->data = nullptr;
        this->n = 0;
    }

    /// Get values from specified indices
    ///
    /// @tparam N Size of the index array
    /// @param idx Indices to get the values from
    /// @return Vector with the value from locations specified by `idx`
    template <Int N>
    [[deprecated("")]] DenseVector<T, N>
    get_values(DenseVector<Int, N> idx) const
    {
        DenseVector<T, N> res;
        for (Int i = 0; i < N; ++i)
            res(i) = get(idx(i));
        return res;
    }

    template <Int N>
    [[deprecated("")]] DenseVector<T, N>
    get_values(const std::vector<Int> & idx) const
    {
        assert(N == idx.size());
        DenseVector<T, N> res;
        for (Int i = 0; i < N; ++i)
            res(i) = get(idx[i]);
        return res;
    }

    /// Assign a value into all vector entries, i.e. `vec[i] = val`
    ///
    /// @param val Value to assign
    void
    set_values(const T & val)
    {
        assert(this->data != nullptr);
        for (Int i = 0; i < this->n; ++i)
            this->data[i] = val;
    }

    /// Set multiple values into the vector
    ///
    /// @param vals Values to set into this vector
    void
    set_values(const std::vector<T> & vals)
    {
        assert(this->data != nullptr);
        assert(this->n == vals.size());
        for (Int i = 0; i < this->n; ++i)
            this->data[i] = vals[i];
    }

    /// Set multiple values at specified indices
    ///
    /// @tparam N Size of the array
    /// @param idx Indices where the values are to be set
    /// @param a Values to be set
    template <Int N>
    [[deprecated("")]] void
    set_values(const DenseVector<Int, N> & idx, const DenseVector<T, N> & a)
    {
        for (Int i = 0; i < N; ++i)
            set(idx(i)) = a(i);
    }

    /// Add values to specified locations
    ///
    /// @tparam N Size of the index and value array
    /// @param idx Indices to modify
    /// @param a Vector with values to add at locations specified by `idx`
    template <Int N>
    [[deprecated("")]] void
    add(const DenseVector<Int, N> & idx, const DenseVector<T, N> & a)
    {
        for (Int i = 0; i < N; ++i)
            set(idx(i)) += a(i);
    }

    // operators

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

    const T &
    operator[](Int i) const
    {
        return get(i);
    }

    T &
    operator[](Int i)
    {
        return set(i);
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
        return Iterator(*this, 0);
    }

    Iterator
    end()
    {
        return Iterator(*this, this->n);
    }

private:
    /// Number of entries in the array
    Int n;
    /// Array containing the values
    T * data;
    /// Range of valid indices
    Range range;
};

template <>
inline void
Array1D<Real>::zero()
{
    assert(this->data != nullptr);
    for (Int i = 0; i < this->n; ++i)
        this->data[i] = 0.;
}

// Output

template <typename T>
std::ostream &
operator<<(std::ostream & os, const Array1D<T> & obj)
{
    os << "(";
    for (Int i = 0; i < obj.get_size(); ++i) {
        os << obj(i);
        if (i < obj.get_size() - 1)
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
    assert(N == idx.size());
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

} // namespace godzilla
