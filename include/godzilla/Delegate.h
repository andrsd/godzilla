// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Ref.h"
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

    template <typename F>
    Delegate(F && f) : fn(std::forward<F>(f))
    {
    }

    template <typename T>
    [[deprecated("")]] void
    bind(T * t, R (T::*method)(ARGS...))
    {
        this->fn = [=](ARGS... args) {
            return (t->*method)(args...);
        };
    }

    template <typename T>
    [[deprecated("")]] void
    bind(T * t, R (T::*method)(ARGS...) const)
    {
        this->fn = [=](ARGS... args) {
            return (t->*method)(args...);
        };
    }

    template <typename T>
    void
    bind(Ref<T> t, R (T::*method)(ARGS...))
    {
        this->fn = [=](ARGS... args) {
            return (*t.*method)(args...);
        };
    }

    template <typename T>
    void
    bind(Ref<T> t, R (T::*method)(ARGS...) const)
    {
        this->fn = [=](ARGS... args) {
            return (*t.*method)(args...);
        };
    }

    template <typename F>
    void
    bind(F && f)
    {
        fn = std::forward<F>(f);
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
    operator bool() { return static_cast<bool>(this->fn); }

private:
    std::function<R(ARGS...)> fn;
};

} // namespace godzilla
