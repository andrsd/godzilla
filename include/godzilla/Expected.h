// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <utility>
#include <cassert>

namespace godzilla {

template <class E>
class Unexpected {
public:
    constexpr explicit Unexpected(E e) : error_(std::move(e)) {}

    constexpr const E &
    value() const &
    {
        return this->error_;
    }

    constexpr E &
    value() &
    {
        return this->error_;
    }

private:
    E error_;
};

template <class T, class E>
class Expected {
public:
    constexpr Expected(const T & v) : has_(true) { new (&this->storage_.value) T(v); }

    constexpr Expected(T && v) : has_(true) { new (&this->storage_.value) T(std::move(v)); }

    constexpr Expected(Unexpected<E> e) : has_(false)
    {
        new (&this->storage_.error) E(std::move(e.value()));
    }

    ~Expected() { reset(); }

    constexpr bool
    has_value() const noexcept
    {
        return this->has_;
    }

    constexpr explicit
    operator bool() const noexcept
    {
        return this->has_;
    }

    constexpr T &
    value() &
    {
        assert(this->has);
        return this->storage_.value;
    }

    constexpr const T &
    value() const &
    {
        assert(this->has);
        return this->storage_.value;
    }

    constexpr E &
    error() &
    {
        assert(!this->has);
        return this->storage_.error;
    }

    constexpr const E &
    error() const &
    {
        assert(!this->has);
        return this->storage_.error;
    }

private:
    void
    reset()
    {
        if (this->has_)
            this->storage_.value.~T();
        else
            this->storage_.error.~E();
    }

    bool has_;
    union Storage {
        T value;
        E error;
        Storage() {}
        ~Storage() {}
    } storage_;
};

} // namespace godzilla
