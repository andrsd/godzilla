// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Assert.h"
#include "godzilla/CallStack.h"
#include <cstdint>
#include <utility>
#include <cstddef>
#include <type_traits>

namespace godzilla {

template <typename T>
class Ref;

//

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

struct Control {
    uint32_t generation : 31;
    // true if borrowed for mutation
    bool mut_borrowed : 1;
    // const borrows
    uint32_t borrow_count;

    Control() : generation(1), mut_borrowed(0), borrow_count(0) {}
};

/// This behaves like std::unique_ptr
template <typename T, typename Deleter = DefaultDelete<T>>
class Qtr {
public:
    constexpr Qtr() noexcept : ptr_(nullptr), deleter_(Deleter()), ctrl_(nullptr) {}
    constexpr Qtr(std::nullptr_t) noexcept : Qtr() {}
    explicit Qtr(T * ptr) noexcept : ptr_(ptr), deleter_(Deleter()), ctrl_(new Control()) {}
    Qtr(T * ptr, Deleter d) noexcept : ptr_(ptr), deleter_(std::move(d)), ctrl_(new Control()) {}

    Qtr(const Qtr &) = delete;
    Qtr & operator=(const Qtr &) = delete;

    Qtr(Qtr && other) noexcept :
        ptr_(std::exchange(other.ptr_, nullptr)),
        deleter_(std::move(other.deleter_)),
        ctrl_(std::exchange(other.ctrl_, nullptr))
    {
        CALL_STACK_MSG();
    }

    template <class U,
              class E,
              class = std::enable_if_t<std::is_convertible<U *, T *>::value &&
                                       std::is_constructible<Deleter, E &&>::value>>
    Qtr(Qtr<U, E> && other) noexcept :
        ptr_(other.release()),
        deleter_(std::forward<E>(other.deleter_)),
        ctrl_(std::exchange(other.ctrl_, nullptr))
    {
    }

    Qtr &
    operator=(Qtr && other) noexcept
    {
        CALL_STACK_MSG();
        if (this != &other) {
            reset();
            this->ptr_ = std::exchange(other.ptr_, nullptr);
            this->deleter_ = std::move(other.deleter_);
            this->ctrl_ = std::exchange(other.ctrl_, nullptr);
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
        this->ctrl_ = std::exchange(other.ctrl_, nullptr);
        return *this;
    }

    ~Qtr()
    {
        CALL_STACK_MSG();
#ifndef NDEBUG
        if (this->ctrl_) {
            assert_true(this->ctrl_->borrow_count == 0, "Active borrow detected");
            assert_true(!this->ctrl_->mut_borrowed, "Active mutable borrow detected");

            this->ctrl_->generation++;
            delete this->ctrl_;
        }
#endif
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

    Ref<T>
    borrow()
    {
#ifndef NDEBUG
        assert_true(!this->ctrl_->mut_borrowed, "Already borrowed for mutation");
        assert_true(this->ctrl_->borrow_count == 0, "Already borrowed");
        this->ctrl_->mut_borrowed = true;
        return Ref<T>(this->ptr_, this->ctrl_, this->ctrl_->generation);
#else
        return Ref<T>(this->ptr_);
#endif
    }

    Ref<const T>
    borrow() const
    {
#ifndef NDEBUG
        assert_true(!this->ctrl_->mut_borrowed, "");
        this->ctrl_->borrow_count++;
        return Ref<const T>(this->ptr_, this->ctrl_, this->ctrl_->generation);
#else
        return Ref<const T>(this->ptr_);
#endif
    }

    Ref<const T>
    borrow_const()
    {
#ifndef NDEBUG
        assert_true(!this->ctrl_->mut_borrowed, "");
        this->ctrl_->borrow_count++;
        return Ref<const T>(this->ptr_, this->ctrl_, this->ctrl_->generation);
#else
        return Ref<const T>(this->ptr_);
#endif
    }

private:
    T * ptr_;
    Deleter deleter_;
    Control * ctrl_;

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

template <typename T>
class Ref {
public:
#ifndef NDEBUG
    Ref() : ptr_(nullptr), ctrl_(nullptr), generation_(1) {}

    // Construct from `nullptr`
    Ref(std::nullptr_t) : ptr_(nullptr), ctrl_(nullptr), generation_(1) {}

    // Copy constructor
    Ref(const Ref & other) : ptr_(other.ptr_), ctrl_(other.ctrl_), generation_(other.generation_) {}

    // Copy assignment
    Ref &
    operator=(const Ref & other)
    {
        if (this->ptr_ != nullptr) {
            if constexpr (std::is_const<T>::value) {
                this->ctrl_->borrow_count--;
            }
            else {
                this->ctrl_->mut_borrowed = false;
            }
        }
        this->ptr_ = other.ptr_;
        this->ctrl_ = other.ctrl_;
        this->generation_ = other.generation_;
        return *this;
    }

    // Move constructor
    Ref(Ref && other) noexcept :
        ptr_(std::exchange(other.ptr_, nullptr)),
        ctrl_(std::exchange(other.ctrl_, nullptr)),
        generation_(std::exchange(other.generation_, 1))
    {
    }

    // Move assignment
    Ref &
    operator=(Ref && other) noexcept
    {
        if (this->ptr_ != nullptr) {
            if constexpr (std::is_const<T>::value) {
                this->ctrl_->borrow_count--;
            }
            else {
                this->ctrl_->mut_borrowed = false;
            }
        }
        this->ptr_ = std::exchange(other.ptr_, nullptr);
        this->ctrl_ = std::exchange(other.ctrl_, nullptr);
        this->generation_ = std::exchange(other.generation_, 1);
        return *this;
    }

    ~Ref()
    {
        if (this->ptr_ != nullptr) {
            assert_true(this->ctrl_->generation == this->generation_, "");

            if constexpr (std::is_const<T>::value) {
                this->ctrl_->borrow_count--;
            }
            else {
                this->ctrl_->mut_borrowed = false;
            }
        }
    }

#else
    Ref() : ptr_(nullptr) {}
    Ref(std::nullptr_t) : ptr_(nullptr) {}
#endif

    T *
    operator->()
    {
        assert_true(this->ctrl_->generation == this->generation_, "Use after free");
        return this->ptr_;
    }

    void
    reset()
    {
        if (this->ptr_ != nullptr) {
            if constexpr (std::is_const<T>::value) {
                this->ctrl_->borrow_count--;
            }
            else {
                this->ctrl_->mut_borrowed = false;
            }
            this->ptr_ = nullptr;
            this->ctrl_ = nullptr;
            this->generation_ = 1;
        }
    }

private:
#ifndef NDEBUG
    Ref(T * ptr, Control * ctrl, uint32_t generation) :
        ptr_(ptr),
        ctrl_(ctrl),
        generation_(generation)
    {
    }
#else
    Ref(T * ptr) : ptr_(ptr) {}
#endif

    T * ptr_;
#ifndef NDEBUG
    Control * ctrl_;
    uint32_t generation_;
#endif

    template <typename U, typename UDeleter>
    friend class Qtr;
};

} // namespace godzilla
