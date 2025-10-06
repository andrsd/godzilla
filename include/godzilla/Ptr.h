// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Exception.h"
#include <utility>
#include <iostream>

namespace godzilla {

/// Reference counted pointer.  It works like `std::shared_ptr<T>`
///
/// @tparam T C++ type we point to
template <typename T>
class Ptr {
private:
    struct ControlBlock {
        T * ptr;
        std::size_t strong;
        std::size_t weak;

        explicit ControlBlock(T * p) : ptr(p), strong(1), weak(0) {}
    };

    ControlBlock * ctrl_;

public:
    Ptr() : ctrl_(nullptr) {
        std::cerr << "default ctor" << std::endl;

    }

    // Construct from `nullptr`
    Ptr(std::nullptr_t) : ctrl_(nullptr) {
        std::cerr << "nullptr ctor" << std::endl;

    }

    // Cross-type constructor (Ptr<U> -> Ptr<T>)
    template <typename U>
    Ptr(const Ptr<U> & other, T * casted) : ctrl_(nullptr)
    {
        std::cerr << "cross-type ctor" << std::endl;
        if (casted) {
            this->ctrl_ = reinterpret_cast<ControlBlock *>(other.ctrl_);
            if (this->ctrl_)
                ++this->ctrl_->strong;
        }
    }

    // Copy constructor
    Ptr(const Ptr & other) : ctrl_(other.ctrl_)
    {
        std::cerr << "copy-ctor" << std::endl;
        if (this->ctrl_)
            ++this->ctrl_->strong;
    }

    // Move constructor
    Ptr(Ptr && other) noexcept : ctrl_(other.ctrl_) { other.ctrl_ = nullptr; }

    // Converting copy constructor
    template <typename U, typename = std::enable_if_t<std::is_convertible<U *, T *>::value>>
    Ptr(const Ptr<U> & other) : ctrl_(reinterpret_cast<ControlBlock *>(other.ctrl_))
    {
        std::cerr << "converting copy-ctor:" << ctrl_ << ", " << other.ctrl_ << std::endl;
        // std::cerr << "converting copy-ctor:" << std::endl;
        if (this->ctrl_) {
            // this->ctrl_ = reinterpret_cast<ControlBlock *>(other.ctrl_);
            ++this->ctrl_->strong;
        }
    }

    // Copy assignment
    Ptr &
    operator=(const Ptr & other)
    {
        std::cerr << "assign" << std::endl;
        if (this != &other) {
            release();
            this->ctrl_ = other.ctrl_;
            if (this->ctrl_)
                ++this->ctrl_->strong;
        }
        return *this;
    }

    // Converting assignment
    template <typename U, typename = std::enable_if_t<std::is_convertible<U *, T *>::value>>
    Ptr &
    operator=(const Ptr<U> & other)
    {
        std::cerr << "convert assign" << std::endl;
        if (reinterpret_cast<const void *>(this) != reinterpret_cast<const void *>(&other)) {
            release();
            this->ctrl_ = reinterpret_cast<ControlBlock *>(other.ctrl_);
            if (this->ctrl_)
                ++this->ctrl_->strong;
        }
        return *this;
    }

    // Assignment from `nullptr`
    Ptr &
    operator=(std::nullptr_t)
    {
        std::cerr << "assign nullptr" << std::endl;
        release();
        this->ctrl_ = nullptr;
        return *this;
    }

    // Move assignment
    Ptr &
    operator=(Ptr && other) noexcept
    {
        std::cerr << "move assign" << std::endl;
        if (this != &other) {
            release();
            this->ctrl_ = other.ctrl_;
            other.ctrl_ = nullptr;
        }
        return *this;
    }

    ~Ptr() { release(); }

    explicit
    operator bool() const
    {
        return get() != nullptr;
    }

    /// Dereference the pointer
    T &
    operator*() const
    {
        if (this->ctrl_)
            return *this->ctrl_->ptr;
        else
            throw Exception("Access into a null pointer");
    }

    // Access the pointer
    T *
    operator->() const
    {
        return this->ctrl_->ptr;
    }

    // Compare two Ptr<T> of the same type
    bool
    operator==(const Ptr & other) const
    {
        return get() == other.get();
    }

    bool
    operator!=(const Ptr & other) const
    {
        return get() != other.get();
    }

    // Compare two Ptr<U> of different types
    template <typename U>
    bool
    operator==(const Ptr<U> & other) const
    {
        return get() == other.get();
    }

    template <typename U>
    bool
    operator!=(const Ptr<U> & other) const
    {
        return get() != other.get();
    }

    // Compare against nullptr
    bool
    operator==(std::nullptr_t) const
    {
        return get() == nullptr;
    }

    bool
    operator!=(std::nullptr_t) const
    {
        return get() != nullptr;
    }

    // Operator <
    bool
    operator<(std::nullptr_t) const
    {
        return get() < nullptr;
    }

    bool
    operator<(const Ptr & other) const
    {
        return get() < other.get();
    }

    template <typename U>
    bool
    operator<(const Ptr<U> & other) const
    {
        return get() < other.get();
    }

    // Get the pointer
    T *
    get() const
    {
        return this->ctrl_ ? this->ctrl_->ptr : nullptr;
    }

    // Get the reference count
    int
    ref_count() const
    {
        return this->ctrl_ ? this->ctrl_->strong : 0;
    }

    // Is this a null pointer?
    bool
    is_null() const
    {
        return this->ctrl_ == nullptr;
    }

private:
    explicit Ptr(T * ptr) : ctrl_(new ControlBlock(ptr)) {}

    void
    release()
    {
        std::cerr << "release()" << std::endl;
        if (this->ctrl_ && --this->ctrl_->strong == 0) {
            std::cerr << "- delete this->ctrl_->ptr" << std::endl;
            delete this->ctrl_->ptr;
            this->ctrl_->ptr = nullptr;
            if (this->ctrl_->weak == 0) {
                std::cerr << "- delete this->ctrl_" << std::endl;
                delete this->ctrl_;
                this->ctrl_ = nullptr;
            }
        }
    }

public:
    /// Allocate a new object and return a `Ptr` to it
    ///
    /// @tparam T C++ type we point to
    /// @tparam ARGS
    /// @param args Arguments passed into a constructor
    template <typename... ARGS>
    static Ptr<T>
    alloc(ARGS &&... args)
    {
        return Ptr<T>(new T(std::forward<ARGS>(args)...));
    }

    template <typename U>
    static Ptr<T>
    cast(const Ptr<U> & other)
    {
        T * casted = static_cast<T *>(other.get());
        return Ptr<T>(other, casted);
    }

    template <typename U>
    static Ptr<T>
    downcast(const Ptr<U> & other)
    {
        T * casted = dynamic_cast<T *>(other.get());
        if (casted)
            return Ptr<T>(other, casted);
        return nullptr;
    }

    template <typename U>
    friend class Ptr;
};

template <typename T, typename U>
Ptr<T>
static_ptr_cast(const Ptr<U> & other)
{
    T * casted = static_cast<T *>(other.get());
    return Ptr<T>(other, casted);
}

template <typename T, typename U>
Ptr<T>
dynamic_ptr_cast(const Ptr<U> & other)
{
    T * casted = dynamic_cast<T *>(other.get());
    if (casted)
        return Ptr<T>(other, casted);
    return nullptr;
}

} // namespace godzilla
