// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Assert.h"

namespace godzilla {

/// Memory arena stores contiguous memory
template <typename T>
class MemoryArena {
public:
    using Marker = size_t;

    /// Create an arena with `capacity` elements
    explicit MemoryArena(std::size_t capacity) :
        capacity_(capacity),
        buffer_(new T[capacity]),
        offset_(0)
    {
    }

    ~MemoryArena() { delete[] this->buffer_; }

    /// Allocate `n` entries from arena
    T *
    allocate(std::size_t n)
    {
        expect_true(this->offset_ + n <= this->capacity_, "Arena out of memory");
        T * ptr = this->buffer_ + this->offset_;
        this->offset_ += n;
        return ptr;
    }

    /// Reset the arena to start allocating from the beginning
    void
    reset()
    {
        this->offset_ = 0;
    }

    /// Get marker pointing at the begining of free space in arena
    Marker
    mark() const
    {
        return this->offset_;
    }

    /// Set internal allocation point to marker
    void
    rewind(Marker m)
    {
        this->offset_ = m;
    }

private:
    std::size_t capacity_;
    T * buffer_;
    std::size_t offset_;
};

} // namespace godzilla
