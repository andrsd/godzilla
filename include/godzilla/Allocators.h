// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/MemoryArena.h"
#include <concepts>
#include <type_traits>

namespace godzilla {

/// Allocator concept
template <typename A, typename T>
concept AllocatorFor = requires(A a, std::size_t n, T * p) {
    { a.allocate(n) } -> std::same_as<T *>;
    { a.deallocate(p, n) };
};

/// Ordinary allocator
template <typename T>
struct DefaultAllocator {
    T *
    allocate(std::size_t n)
    {
        return static_cast<T *>(::operator new[](n * sizeof(T)));
    }
    void
    deallocate(T * p, std::size_t)
    {
        ::operator delete[](p);
    }
};

/// Memory arena allocator
template <typename T>
class MemoryArenaAllocator {
public:
    using value_type = T;

    explicit MemoryArenaAllocator(MemoryArena<T> & arena) noexcept : arena_(&arena) {}

    template <typename U>
    MemoryArenaAllocator(const MemoryArenaAllocator<U> & other) noexcept : arena_(other.arena_)
    {
    }

    T *
    allocate(std::size_t n)
    {
        return this->arena_->allocate(n);
    }

    void
    deallocate(T * ptr, std::size_t n)
    {
        // no-op, arena frees everything at once
    }

private:
    MemoryArena<T> * arena_;
};

} // namespace godzilla
