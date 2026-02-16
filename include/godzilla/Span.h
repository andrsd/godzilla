// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Assert.h"
#include "godzilla/Types.h"
#include <iterator>
#include <type_traits>

namespace godzilla {

template <typename T>
class Span {
public:
    using element_type = T;
    using value_type = std::remove_cv_t<T>;
    using size_type = Int;
    using difference_type = int;
    using pointer = T *;
    using const_pointer = const T *;
    using reference = T &;
    using const_reference = const T &;
    using iterator = T *;
    using const_iterator = const T *;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    // Constructors
    constexpr Span() noexcept : data_(nullptr), size_(0) {}

    constexpr Span(pointer ptr, size_type count) : data_(ptr), size_(count) {}

    constexpr Span(pointer first, pointer last) :
        data_(first),
        size_(static_cast<size_type>(last - first))
    {
    }

    template <std::size_t N>
    constexpr Span(element_type (&arr)[N]) noexcept : data_(arr), size_(static_cast<size_type>(N))
    {
    }

    template <typename Container,
              typename = std::enable_if_t<
                  !std::is_array_v<Container> &&
                  std::is_convertible_v<decltype(std::declval<Container>().data()), pointer>>>
    constexpr Span(Container & cont) :
        data_(cont.data()),
        size_(static_cast<size_type>(cont.size()))
    {
    }

    template <typename Container,
              typename = std::enable_if_t<
                  !std::is_array_v<Container> &&
                  std::is_convertible_v<decltype(std::declval<const Container>().data()), pointer>>>
    constexpr Span(const Container & cont) :
        data_(cont.data()),
        size_(static_cast<size_type>(cont.size()))
    {
    }

    constexpr Span(const Span &) noexcept = default;
    constexpr Span & operator=(const Span &) noexcept = default;

    constexpr iterator
    begin() const noexcept
    {
        return this->data_;
    }

    constexpr iterator
    end() const noexcept
    {
        return this->data_ + this->size_;
    }

    constexpr const_iterator
    cbegin() const noexcept
    {
        return this->data_;
    }

    constexpr const_iterator
    cend() const noexcept
    {
        return this->data_ + this->size_;
    }

    constexpr reverse_iterator
    rbegin() const noexcept
    {
        return reverse_iterator(end());
    }

    constexpr reverse_iterator
    rend() const noexcept
    {
        return reverse_iterator(begin());
    }

    constexpr const_reverse_iterator
    crbegin() const noexcept
    {
        return const_reverse_iterator(cend());
    }

    constexpr const_reverse_iterator
    crend() const noexcept
    {
        return const_reverse_iterator(cbegin());
    }

    constexpr reference
    operator[](size_type idx) const
    {
        GODZILLA_ASSERT_TRUE(
            idx >= 0 && idx < this->size_,
            fmt::format("Span::operator[]: Index {} out of range ({})", idx, this->size_));
        return this->data_[idx];
    }

    constexpr reference
    at(size_type idx) const
    {
        GODZILLA_ASSERT_TRUE(idx >= 0 && idx < this->size_,
                             fmt::format("Span::at: Index {} out of range ({})", idx, this->size_));
        return this->data_[idx];
    }

    constexpr reference
    front() const
    {
        return this->data_[0];
    }

    constexpr reference
    back() const
    {
        return this->data_[this->size_ - 1];
    }

    constexpr pointer
    data() const noexcept
    {
        return this->data_;
    }

    constexpr size_type
    size() const noexcept
    {
        return this->size_;
    }

    constexpr size_type
    size_bytes() const noexcept
    {
        return this->size_ * static_cast<size_type>(sizeof(element_type));
    }

    constexpr bool
    empty() const noexcept
    {
        return this->size_ == 0;
    }

    constexpr Span
    first(size_type count) const
    {
        return Span(this->data_, count);
    }

    constexpr Span
    last(size_type count) const
    {
        return Span(this->data_ + (this->size_ - count), count);
    }

    constexpr Span
    subspan(size_type offset, size_type count = -1) const
    {
        if (count == -1) {
            count = this->size_ - offset;
        }
        return Span(this->data_ + offset, count);
    }

private:
    pointer data_;
    size_type size_;
};

// Deduction guides
template <typename T, std::size_t N>
Span(T (&)[N]) -> Span<T>;

template <typename Container>
Span(Container &) -> Span<typename Container::value_type>;

template <typename Container>
Span(const Container &) -> Span<const typename Container::value_type>;

} // namespace godzilla
