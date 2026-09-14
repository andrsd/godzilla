// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "godzilla/Range.h"
#include "godzilla/Exception.h"
#include "godzilla/Assert.h"
#include "godzilla/Math.h"
#include "mpicpp-lite/mpicpp-lite.h"
#include <petscvec.h>
#include <cstring>

namespace mpi = mpicpp_lite;

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

        explicit Iterator(const Array1D * arr, Int idx) : arr_(arr), idx_(idx) {}

        value_type &
        operator*() const
        {
            return *(this->arr_->data_ + this->idx_);
        }

        /// Prefix increment
        Iterator &
        operator++()
        {
            ++this->idx_;
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
            return (a.arr_ == b.arr_) && (a.idx_ == b.idx_);
        };

        friend bool
        operator!=(const Iterator & a, const Iterator & b)
        {
            return (a.arr_ != b.arr_) || (a.idx_ != b.idx_);
        };

    private:
        /// Array to iterate over
        const Array1D * arr_;
        /// Index pointing into the array
        Int idx_;
    };

    struct ConstIterator {
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using pointer = const T *;
        using reference = const T &;

        explicit ConstIterator(const Array1D * arr, Int idx) : arr_(arr), idx_(idx) {}

        const value_type &
        operator*() const
        {
            return *(this->arr_->data_ + this->idx_);
        }

        /// Prefix increment
        ConstIterator &
        operator++()
        {
            ++this->idx_;
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
            return (a.arr_ == b.arr_) && (a.idx_ == b.idx_);
        };

        friend bool
        operator!=(const ConstIterator & a, const ConstIterator & b)
        {
            return (a.arr_ != b.arr_) || (a.idx_ != b.idx_);
        };

    private:
        /// Array to iterate over
        const Array1D * arr_;
        /// Index pointing into the array
        Int idx_;
    };

    /// Create an empty array
    Array1D() : ctrl_(nullptr), first_(0), data_(nullptr) {}

    /// Create an empty array
    explicit Array1D(mpi::Communicator comm) :
        comm_(comm),
        ctrl_(nullptr),
        first_(0),
        data_(nullptr)
    {
    }

    explicit Array1D(mpi::Communicator comm, Int size) :
        comm_(comm),
        ctrl_(new ControlBlock { 1, size }),
        first_(0),
        data_(new T[size])
    {
    }

    explicit Array1D(mpi::Communicator comm, const Range & rng) :
        comm_(comm),
        ctrl_(new ControlBlock { 1, rng.size() }),
        first_(rng.first()),
        data_(new T[rng.size()])
    {
    }

    ~Array1D() { release(); }

    // Copy constructor
    Array1D(const Array1D & other) : ctrl_(other.ctrl_), first_(other.first_), data_(other.data_)
    {
        if (this->ctrl_)
            ++this->ctrl_->ref_count;
    }

    // Copy assignment
    Array1D &
    operator=(const Array1D & other)
    {
        if (this != &other) {
            release();
            this->ctrl_ = other.ctrl_;
            this->first_ = other.first_;
            this->data_ = other.data_;
            if (this->ctrl_)
                ++this->ctrl_->ref_count;
        }
        return *this;
    }

    // Move constructor
    Array1D(Array1D && other) noexcept :
        ctrl_(std::exchange(other.ctrl_, nullptr)),
        first_(std::exchange(other.first_, 0)),
        data_(std::exchange(other.data_, nullptr))
    {
    }

    // Move assignment
    Array1D &
    operator=(Array1D && other) noexcept
    {
        if (this != &other) {
            release();
            this->ctrl_ = std::exchange(other.ctrl_, nullptr);
            this->first_ = std::exchange(other.first_, 0);
            this->data_ = std::exchange(other.data_, nullptr);
        }
        return *this;
    }

    explicit
    operator bool() const
    {
        return this->data_ != nullptr;
    }

    mpi::Communicator
    get_comm() const
    {
        return this->comm_;
    }

    /// Get number of entries in the array
    ///
    /// @return Number of entries in the array
    Int
    size() const
    {
        return this->ctrl_ ? this->ctrl_->n : 0;
    }

    /// Set all entries in the array to zero
    void
    zero()
    {
        GODZILLA_ASSERT_TRUE(this->data != nullptr, "Internal storage is not allocated");
        for (Int i = 0; i < this->ctrl_->n; ++i)
            this->data_[i].zero();
    }

    /// Assign a value into all vector entries, i.e. `vec[i] = val`
    ///
    /// @param val Value to assign
    void
    set(const T & val)
    {
        GODZILLA_ASSERT_TRUE(this->data != nullptr, "Internal storage is not allocated");
        for (Int i = 0; i < this->ctrl_->n; ++i)
            this->data_[i] = val;
    }

    // operators

    /// Get the entry at a specified location for reading
    ///
    /// @param i Index fo the entry
    /// @return Entry at the `ith` location
    const T &
    operator[](Int i) const
    {
        GODZILLA_ASSERT_TRUE(this->data != nullptr, "Internal storage is not allocated");
        GODZILLA_ASSERT_TRUE((i >= this->first) && (i < this->first + this->ctrl->n),
                             "Index out of bounds");
        auto idx = i - this->first_;
        return this->data_[idx];
    }

    /// Get the entry at a specified location for writing
    ///
    /// @param i Index fo the entry
    /// @return Entry at the `ith` location
    T &
    operator[](Int i)
    {
        GODZILLA_ASSERT_TRUE(this->data != nullptr, "Internal storage is not allocated");
        GODZILLA_ASSERT_TRUE((i >= this->first) && (i < this->first + this->ctrl->n),
                             "Index out of bounds");
        auto idx = i - this->first_;
        return this->data_[idx];
    }

    //

    // Do your best to avoid abusing this API
    T *
    get_data()
    {
        return this->data_;
    }

    const T *
    get_data() const
    {
        return this->data_;
    }

    Iterator
    begin()
    {
        return Iterator(this, 0);
    }

    Iterator
    end()
    {
        return Iterator(this, this->ctrl_->n);
    }

    ConstIterator
    begin() const
    {
        return ConstIterator(this, 0);
    }

    ConstIterator
    end() const
    {
        return ConstIterator(this, this->ctrl_->n);
    }

private:
    void
    release()
    {
        if (this->ctrl_ && --this->ctrl_->ref_count == 0) {
            delete this->ctrl_;
            delete[] this->data_;
        }
    }

    mpi::Communicator comm_;
    /// Control block
    ControlBlock * ctrl_;
    /// First index
    Int first_;
    /// Array containing the values
    T * data_;

    template <FloatingPoint U>
    friend U norm(Array1D<U> & vector, NormType type);
    template <typename U>
    friend void pointwise_min(Array1D<U> & w, const Array1D<U> & x, const Array1D<U> & y);
    template <typename U>
    friend void pointwise_max(Array1D<U> & w, const Array1D<U> & x, const Array1D<U> & y);
    template <typename U>
    friend void pointwise_mult(Array1D<U> & w, const Array1D<U> & x, const Array1D<U> & y);
    template <typename U>
    friend void pointwise_divide(Array1D<U> & w, const Array1D<U> & x, const Array1D<U> & y);
};

template <>
inline void
Array1D<Real>::zero()
{
    GODZILLA_ASSERT_TRUE(this->data != nullptr, "Internal storage is not allocated");
    for (Int i = 0; i < this->ctrl_->n; ++i)
        this->data_[i] = 0.;
}

// Output

template <typename T>
std::ostream &
operator<<(std::ostream & os, const Array1D<T> & obj)
{
    os << "(";
    for (Int i = 0; i < obj.size(); ++i) {
        os << obj[i];
        if (i < obj.size() - 1)
            os << ", ";
    }
    os << ")";
    return os;
}

template <typename T>
void
copy(const Array1D<T> & src, Array1D<T> & dest)
{
    GODZILLA_ASSERT_TRUE(src.size() == dest.size(),
                         "Size of `src` argument does not match the size of the `dest`");
    std::memcpy(dest.get_data(), src.get_data(), sizeof(T) * src.size());
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
    GODZILLA_ASSERT_TRUE(N == idx.size(),
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
    GODZILLA_ASSERT_TRUE(data.size() == vals.size(),
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
        vector.comm_.all_reduce(norm, mpi::op::sum<T>());
        return norm;

    case NORM_2:
        for (const auto & val : vector)
            norm += val * val;
        vector.comm_.all_reduce(norm, mpi::op::sum<T>());
        return std::sqrt(norm);

    case NORM_INFINITY:
        for (const auto & val : vector)
            norm = std::max(norm, std::abs(val));
        vector.comm_.all_reduce(norm, mpi::op::max<T>());
        return norm;

    default:
        throw NotImplementedException(
            format("`norm` not implemented for type {}", static_cast<int>(type)));
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
    GODZILLA_ASSERT_TRUE(w.size() == x.size(), "The size of 'w' does not match the size 'x'");
    GODZILLA_ASSERT_TRUE(w.size() == y.size(), "The size of 'w' does not match the size 'y'");
    for (Int i = 0; i < w.ctrl_->n; ++i)
        w.data_[i] = math::min(x.data_[i], y.data_[i]);
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
    GODZILLA_ASSERT_TRUE(w.size() == x.size(), "The size of 'w' does not match the size 'x'");
    GODZILLA_ASSERT_TRUE(w.size() == y.size(), "The size of 'w' does not match the size 'y'");
    for (Int i = 0; i < w.ctrl_->n; ++i)
        w.data_[i] = math::max(x.data_[i], y.data_[i]);
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
    GODZILLA_ASSERT_TRUE(w.size() == x.size(), "The size of 'w' does not match the size 'x'");
    GODZILLA_ASSERT_TRUE(w.size() == y.size(), "The size of 'w' does not match the size 'y'");
    for (Int i = 0; i < w.ctrl_->n; ++i)
        w.data_[i] = x.data_[i] * y.data_[i];
}

/// Compute pointwise division of elements
///
/// @tparam T C++ type
/// @param w Resulting array
/// @param x First array
/// @param y Second array
template <typename T>
void
pointwise_divide(Array1D<T> & w, const Array1D<T> & x, const Array1D<T> & y)
{
    GODZILLA_ASSERT_TRUE(w.size() == x.size(), "The size of 'w' does not match the size 'x'");
    GODZILLA_ASSERT_TRUE(w.size() == y.size(), "The size of 'w' does not match the size 'y'");
    for (Int i = 0; i < w.ctrl_->n; ++i)
        w.data_[i] = x.data_[i] / y.data_[i];
}

} // namespace godzilla
