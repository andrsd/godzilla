// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <utility>
#include <cassert>

namespace godzilla {

template <class E>
class Unexpected {
public:
    constexpr explicit Unexpected(E e) : error(std::move(e)) {}

    constexpr const E &
    value() const &
    {
        return this->error;
    }

    constexpr E &
    value() &
    {
        return this->error;
    }

private:
    E error;
};

template <class T, class E>
class Expected {
public:
    constexpr Expected(const T & v) : has(true) { new (&this->storage.value) T(v); }

    constexpr Expected(T && v) : has(true) { new (&this->storage.value) T(std::move(v)); }

    constexpr Expected(Unexpected<E> e) : has(false)
    {
        new (&this->storage.error) E(std::move(e.value()));
    }

    ~Expected() { reset(); }

    constexpr bool
    has_value() const noexcept
    {
        return this->has;
    }

    constexpr explicit
    operator bool() const noexcept
    {
        return this->has;
    }

    constexpr T &
    value() &
    {
        assert(this->has);
        return this->storage.value;
    }

    constexpr const T &
    value() const &
    {
        assert(this->has);
        return this->storage.value;
    }

    constexpr E &
    error() &
    {
        assert(!this->has);
        return this->storage.error;
    }

    constexpr const E &
    error() const &
    {
        assert(!this->has);
        return this->storage.error;
    }

private:
    void
    reset()
    {
        if (this->has)
            this->storage.value.~T();
        else
            this->storage.error.~E();
    }

    bool has;
    union Storage {
        T value;
        E error;
        Storage() {}
        ~Storage() {}
    } storage;
};

} // namespace godzilla
