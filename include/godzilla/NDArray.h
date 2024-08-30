// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "godzilla/Error.h"
#include <cassert>

namespace godzilla {

template <typename T>
class NDArray {
    template <bool...>
    struct bool_pack {};

    template <class... U>
    using conjunction = std::is_same<bool_pack<true, U::value...>, bool_pack<U::value..., true>>;

    template <typename... U>
    using all_integral = typename conjunction<std::is_integral<U>...>::type;

public:
    NDArray() : rnk(0), dims(), strides(), sz(0), vals(nullptr) {}

    explicit NDArray(const std::vector<Int> & dimens) :
        rnk(dims.size()),
        dims(rnk),
        strides(rnk),
        sz(0),
        vals(nullptr)
    {
        for (Int i = 0; i < rnk; ++i)
            this->dims[i] = dimens[i];
        set_size_and_strides();
        this->vals = new T[this->sz];
    }

    explicit NDArray(const std::vector<Int> & dimens, const T & val) :
        rnk(dims.size()),
        dims(rnk),
        strides(rnk),
        sz(0),
        vals(nullptr)
    {
        for (Int i = 0; i < rnk; ++i)
            this->dims[i] = dimens[i];
        set_size_and_strides();
        this->vals = new T[this->sz];
        for (Int i = 0; i < this->sz; ++i)
            this->vals[i] = val;
    }

    NDArray(const std::initializer_list<Int> & dimens) :
        rnk(dimens.size()),
        dims(rnk),
        strides(rnk),
        sz(0),
        vals(nullptr)
    {
        Int i = 0;
        for (Int d : dimens)
            this->dims[i++] = d;
        set_size_and_strides();
        this->vals = new T[this->sz];
    }

    NDArray(const std::initializer_list<Int> & dimens, const T & val) :
        rnk(dimens.size()),
        dims(rnk),
        strides(rnk),
        sz(0),
        vals(nullptr)
    {
        Int i = 0;
        for (Int d : dimens)
            this->dims[i++] = d;
        set_size_and_strides();
        this->vals = new T[this->sz];
        for (Int i = 0; i < this->sz; ++i)
            this->vals[i] = val;
    }

    ~NDArray()
    {
        delete[] this->vals;
        this->vals = nullptr;
    }

    /// Sets a value to all the items in the array.
    void
    set(const T & value)
    {
        for (Int i = 0; i < this->sz; ++i)
            this->vals[i] = value;
    }

    /// Returns the number of elements in the array.
    Int
    size() const noexcept
    {
        return this->sz;
    }

    /// Returns true if the array has no elements.
    bool
    empty() const noexcept
    {
        return this->sz == 0;
    }

    /// Returns the rank of the array.
    Int
    rank() const noexcept
    {
        return this->rnk;
    }

    /// Returns the dimension of the array.
    const std::vector<Int> &
    dimension() const
    {
        return this->dims;
    }

    /// Accesses the specified element.
    ///
    /// @param args The indices of the desired element.
    /// @return Read/write reference to the element.
    template <typename... Args>
    T &
    operator()(Args... args) noexcept
    {
        static_assert(all_integral<Args...>::value,
                      "NDArray::operator(): All parameters must be of integral type");
        Int indices[] { static_cast<Int>(args)... };
        const Int N = this->rnk;
        T * address = this->vals + indices[N - 1];
        for (Int i = 0; i < N - 1; ++i)
            address += this->strides[i] * indices[i];
        return *(address);
    }

    /// Accesses the specified element.
    ///
    /// @param args The indices of the desired element.
    /// @return Read reference to the element.
    template <typename... Args>
    T const &
    operator()(Args... args) const noexcept
    {
        static_assert(all_integral<Args...>::value,
                      "NDArray::operator(): All parameters must be of integral type");
        Int indices[] { static_cast<Int>(args)... };
        const Int N = this->rnk;
        T * address = this->vals + indices[N - 1];
        for (Int i = 0; i < N - 1; ++i)
            address += this->strides[i] * indices[i];
        return *(address);
    }

private:
    inline void
    set_size_and_strides()
    {
        this->sz = 1;
        for (Int i = 0; i < this->rnk; ++i) {
            assert(this->dims[i] > 0);
            this->sz *= this->dims[i];
            this->strides[i] = 1;
            for (Int j = i + 1; j < this->rnk; ++j)
                this->strides[i] *= this->dims[j];
        }
    }

    /// Rank
    Int rnk;
    /// Dimensions
    std::vector<Int> dims;
    /// Strides per dimensions
    std::vector<Int> strides;
    /// Total number of entries in `vals`
    Int sz;
    /// Values
    T * vals;
};

} // namespace godzilla
