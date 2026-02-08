// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Optional.h"
#include <concepts>
#include <type_traits>
#include <exception>

namespace godzilla {

/// Reference to something
///
/// @tparam T C++ type we are wrapping
template <typename T>
class Ref {
public:
    using element_type = T;

    // Constructor from reference
    explicit Ref(T & ref) noexcept : ptr_(&ref) {}

    // template <typename U>
    //     requires std::is_convertible_v<U *, T *>
    // explicit Ref(U & ref) noexcept : ptr_(&ref)
    // {
    // }

    template <typename U>
        requires std::is_convertible_v<U *, T *>
    Ref(const Ref<U> & other) noexcept : ptr_(other.ptr_)
    {
    }

    // Copy constructor and assignment
    Ref(const Ref &) noexcept = default;
    Ref(Ref &) noexcept = default;
    Ref & operator=(const Ref &) noexcept = default;

    // Pointer-like access
    T *
    operator->() const noexcept
    {
        return this->ptr_;
    }

    T &
    operator*() const noexcept
    {
        return *this->ptr_;
    }

    // Explicit accessor
    T &
    get() const noexcept
    {
        return *this->ptr_;
    }

    bool
    operator==(const Ref & other) const noexcept
    {
        return this->ptr_ == other.ptr_;
    }

    template <typename U>
    bool
    operator==(const Ref<U> & other) const noexcept
    {
        return this->ptr_ == other.ptr_;
    }

    explicit operator bool() const = delete;

private:
    T * ptr_;

    template <typename>
    friend class Ref;
};

template <typename T>
class Ref<const T> {
public:
    explicit Ref(const T & ref) noexcept : ptr_(&ref) {}

    explicit Ref(Ref<T> ref) noexcept : ptr_(ref.ptr_) {}

    template <typename U>
        requires std::is_convertible_v<U *, T *>
    Ref(const Ref<const U> & other) noexcept : ptr_(other.ptr_)
    {
    }

    // Copy constructor and assignment
    Ref(const Ref &) noexcept = default;
    Ref & operator=(const Ref &) noexcept = default;

    // Pointer-like access
    const T *
    operator->() const noexcept
    {
        return ptr_;
    }

    const T &
    operator*() const noexcept
    {
        return *ptr_;
    }

    const T &
    get() const noexcept
    {
        return *this->ptr_;
    }

    bool
    operator==(const Ref & other) const noexcept
    {
        return this->ptr_ == other.ptr_;
    }

    template <typename U>
    bool
    operator==(const Ref<U> & other) const noexcept
    {
        return this->ptr_ == other.ptr_;
    }

    explicit operator bool() const = delete;

private:
    const T * ptr_;

    template <typename>
    friend class Ref;
};

template <typename T>
class LateRef {
public:
    void
    set(Ref<T> r) noexcept
    {
        this->ref_ = r;
    }

    Ref<T>
    get() const
    {
        return *this->ref_;
    }

private:
    Optional<Ref<T>> ref_;

public:
    static LateRef
    from_ref(Ref<T> r)
    {
        LateRef<T> lr;
        lr.ref_ = { r };
        return lr;
    }
};

// Helper functions
template <typename T>
Ref<T>
ref(T & t) noexcept
{
    return Ref<T>(t);
}

template <typename T>
Ref<const T>
cref(const T & t) noexcept
{
    return Ref<const T>(t);
}

template <typename T>
Ref<const T>
cref(Ref<T> t) noexcept
{
    return Ref<const T>(t);
}

template <typename To, typename From>
Ref<To>
dynamic_ref_cast(Ref<From> from)
{
    static_assert(std::is_polymorphic_v<From>, "dynamic_ref_cast requires polymorphic base");

    if (auto p = dynamic_cast<To *>(from.operator->())) {
        return Ref<To>(*p);
    }

    throw std::bad_cast {};
}

template <typename To, typename From>
Optional<Ref<To>>
try_dynamic_ref_cast(Ref<From> from)
{
    if (auto p = dynamic_cast<To *>(from.operator->())) {
        return Ref<To>(*p);
    }
    return std::nullopt;
}

template <typename>
struct is_ref : std::false_type {};

template <typename T>
struct is_ref<Ref<T>> : std::true_type {};

template <typename T>
inline constexpr bool is_ref_v = is_ref<T>::value;

template <typename T>
concept RefType = is_ref_v<T>;

} // namespace godzilla
