// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <functional>

namespace godzilla {

template <typename>
class Delegate;

template <typename R, typename... ARGS>
class Delegate<R(ARGS...)> {
public:
    Delegate() = default;

    template <typename T>
    Delegate(T * t, R (T::*method)(ARGS...))
    {
        this->fn = [=](ARGS... args) {
            return (t->*method)(args...);
        };
    }

    template <typename T>
    void
    bind(T * t, R (T::*method)(ARGS...))
    {
        this->fn = [=](ARGS... args) {
            return (t->*method)(args...);
        };
    }

    R
    operator()(ARGS... args)
    {
        return this->fn(args...);
    }

    R
    invoke(ARGS... args)
    {
        return this->fn(args...);
    }

    void
    reset()
    {
        this->fn = nullptr;
    }

    /// Check whether delegates is callable
    operator bool() const { return static_cast<bool>(this->fn); }

private:
    std::function<R(ARGS...)> fn;
};

} // namespace godzilla
