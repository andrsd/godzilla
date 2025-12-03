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
        capacity(capacity),
        buffer(new T[capacity]),
        offset(0)
    {
    }

    ~MemoryArena() { delete[] this->buffer; }

    /// Allocate `n` entries from arena
    T *
    allocate(std::size_t n)
    {
        expect_true(this->offset + n <= this->capacity, "Arena out of memory");
        T * ptr = this->buffer + this->offset;
        this->offset += n;
        return ptr;
    }

    /// Reset the arena to start allocating from the beginning
    void
    reset()
    {
        this->offset = 0;
    }

    /// Get marker pointing at the begining of free space in arena
    Marker
    mark() const
    {
        return this->offset;
    }

    /// Set internal allocation point to marker
    void
    rewind(Marker m)
    {
        this->offset = m;
    }

private:
    std::size_t capacity;
    T * buffer;
    std::size_t offset;
};

} // namespace godzilla
