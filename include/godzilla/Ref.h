// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

namespace godzilla {

/// Reference wrapper.  It works like `std::reference_wrapper<T>`
///
/// @tparam T C++ type we are wrapping
template <typename T>
class Ref {
public:
    // Constructor from reference
    explicit Ref(T & ref) noexcept : ptr(&ref) {}

    // Copy constructor and assignment
    Ref(const Ref &) noexcept = default;
    Ref & operator=(const Ref &) noexcept = default;

    // Implicit conversion to T&
    operator T &() const noexcept { return *this->ptr; }

    // Explicit accessor
    T &
    get() const noexcept
    {
        return *this->ptr;
    }

    // Comparability (optional but useful for containers)
    bool
    operator==(const Ref & other) const noexcept
    {
        return this->ptr == other.ptr;
    }

private:
    T * ptr;
};

/// Reference wrapper for const types
template <typename T>
class Ref<const T> {
public:
    explicit Ref(const T & ref) noexcept : ptr(&ref) {}

    Ref(const Ref &) noexcept = default;
    Ref & operator=(const Ref &) noexcept = default;

    operator const T &() const noexcept { return *this->ptr; }
    const T &
    get() const noexcept
    {
        return *this->ptr;
    }

    bool
    operator==(const Ref & other) const noexcept
    {
        return this->ptr == other.ptr;
    }

private:
    const T * ptr;
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

} // namespace godzilla
