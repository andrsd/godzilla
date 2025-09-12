// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/CallStack.h"
#include <utility>
#include <cstddef>
#include <type_traits>

namespace godzilla {

template <typename T>
struct DefaultDelete {
    constexpr DefaultDelete() noexcept = default;

    template <typename U, typename = std::enable_if_t<std::is_convertible<U *, T *>::value>>
    DefaultDelete(const DefaultDelete<U> &) noexcept
    {
    }

    void
    operator()(T * ptr) const noexcept
    {
        static_assert(!std::is_void<T>::value, "Can't delete incomplete type");
        delete ptr;
    }
};

// Default deleter for arrays
template <typename T>
struct DefaultDelete<T[]> {
    void
    operator()(T * ptr) const noexcept
    {
        static_assert(!std::is_void<T>::value, "Can't delete incomplete type");
        delete[] ptr;
    }
};

/// This behaves like std::unique_ptr
template <typename T, typename Deleter = DefaultDelete<T>>
class Qtr {
public:
    constexpr Qtr() noexcept : ptr_(nullptr), deleter_(Deleter()) {}
    constexpr Qtr(std::nullptr_t) noexcept : Qtr() {}
    explicit Qtr(T * ptr) noexcept : ptr_(ptr), deleter_(Deleter()) {}
    Qtr(T * ptr, Deleter d) noexcept : ptr_(ptr), deleter_(std::move(d)) {}

    Qtr(const Qtr &) = delete;
    Qtr & operator=(const Qtr &) = delete;

    Qtr(Qtr && other) noexcept : ptr_(other.ptr_), deleter_(std::move(other.deleter_))
    {
        CALL_STACK_MSG();
        other.ptr_ = nullptr;
    }

    template <class U,
              class E,
              class = std::enable_if_t<std::is_convertible<U *, T *>::value &&
                                       std::is_constructible<Deleter, E &&>::value>>
    Qtr(Qtr<U, E> && other) noexcept :
        ptr_(other.release()),
        deleter_(std::forward<E>(other.deleter_))
    {
    }

    Qtr &
    operator=(Qtr && other) noexcept
    {
        CALL_STACK_MSG();
        if (this != &other) {
            reset();
            this->ptr_ = other.ptr_;
            this->deleter_ = std::move(other.deleter_);
            other.ptr_ = nullptr;
        }
        return *this;
    }

    template <class U,
              class E,
              class = std::enable_if_t<std::is_convertible<U *, T *>::value &&
                                       std::is_assignable<Deleter &, E &&>::value>>
    Qtr &
    operator=(Qtr<U, E> && other) noexcept
    {
        reset(other.release());
        this->deleter_ = std::forward<E>(other.deleter_);
        return *this;
    }

    ~Qtr()
    {
        CALL_STACK_MSG();
        reset();
    }

    T *
    get() const noexcept
    {
        CALL_STACK_MSG();
        return this->ptr_;
    }

    bool
    is_null() const noexcept
    {
        CALL_STACK_MSG();
        return this->ptr_ == nullptr;
    }

    explicit
    operator bool() const noexcept
    {
        CALL_STACK_MSG();
        return !is_null();
    }

    bool
    operator==(std::nullptr_t) const noexcept
    {
        CALL_STACK_MSG();
        return is_null();
    }

    bool
    operator!=(std::nullptr_t) const noexcept
    {
        CALL_STACK_MSG();
        return !is_null();
    }

    T &
    operator*() const
    {
        CALL_STACK_MSG();
        return *this->ptr_;
    }

    T *
    operator->() const noexcept
    {
        CALL_STACK_MSG();
        return this->ptr_;
    }

    T *
    release() noexcept
    {
        CALL_STACK_MSG();
        T * tmp = this->ptr_;
        this->ptr_ = nullptr;
        return tmp;
    }

    void
    reset(T * p = nullptr) noexcept
    {
        CALL_STACK_MSG();
        if (this->ptr_ != p) {
            if (this->ptr_)
                this->deleter_(this->ptr_);
            this->ptr_ = p;
        }
    }

    void
    swap(Qtr & other) noexcept
    {
        CALL_STACK_MSG();
        std::swap(this->ptr_, other.ptr_);
        std::swap(this->deleter_, other.deleter_);
    }

private:
    T * ptr_;
    Deleter deleter_;

public:
    template <typename... Args>
    static Qtr<T>
    alloc(Args &&... args)
    {
        CALL_STACK_MSG();
        return Qtr<T>(new T(std::forward<Args>(args)...));
    }

    template <typename U, typename UDeleter>
    friend class Qtr;
};

} // namespace godzilla
