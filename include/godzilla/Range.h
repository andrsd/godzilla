// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"

namespace godzilla {

/// Contiguous range of indices
class Range {
public:
    struct Iterator {
        using iterator_category = std::forward_iterator_tag;
        using value_type = Int;

        explicit Iterator(Int idx) : idx_(idx) {}

        const value_type &
        operator*() const
        {
            return this->idx_;
        }

        /// Prefix increment
        Iterator &
        operator++()
        {
            ++this->idx_;
            return *this;
        }

        /// Postfix increment
        Iterator
        operator++(int)
        {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        Iterator
        operator+(Int n) const
        {
            return Iterator(this->idx_ + n);
        }

        Iterator &
        operator+=(Int n)
        {
            this->idx_ += n;
            return *this;
        }

        friend bool
        operator==(const Iterator & a, const Iterator & b)
        {
            return a.idx_ == b.idx_;
        };

        friend bool
        operator!=(const Iterator & a, const Iterator & b)
        {
            return a.idx_ != b.idx_;
        };

        Int
        operator-(const Iterator & other) const
        {
            return this->idx_ - other.idx_;
        }

        Iterator
        operator-(Int n) const
        {
            return Iterator(this->idx_ - n);
        }

        bool
        operator<(const Iterator & other) const
        {
            return this->idx_ < other.idx_;
        }

    private:
        Int idx_;
    };

    Range() : first_idx_(-1), last_idx_(-1) {}
    Range(Int first, Int last) : first_idx_(first), last_idx_(last) {}

    Iterator
    begin() const
    {
        return Iterator(this->first_idx_);
    }

    Iterator
    end() const
    {
        return Iterator(this->last_idx_);
    }

    /// Get the number of indices in the range
    Int
    size() const
    {
        return last_idx_ - first_idx_;
    }

    /// Get the first index in the range
    ///
    /// @return First index in the range
    Int
    first() const
    {
        return first_idx_;
    }

    /// Get the last index (not included) in the range
    ///
    /// @return Last index (not included) in the range
    Int
    last() const
    {
        return last_idx_;
    }

    /// Test if the range contains a given number
    ///
    /// @param x Number to test
    /// @return `true` if the number is inside the range, `false` otherwise
    bool
    contains(Int x) const
    {
        if (size() == 0)
            return false;
        else
            return (this->first_idx_ <= x) && (x < this->last_idx_);
    }

private:
    /// First index
    Int first_idx_;
    /// Last index (not included in the range)
    Int last_idx_;
};

/// Create a range from `start` to `end`
///
/// @param start First element
/// @param end Last elements (excluded)
inline Range
make_range(Int start, Int end)
{
    return Range(start, end);
}

/// Create a range from 0 to `end`
///
/// @param end Last elements (excluded)
inline Range
make_range(Int end)
{
    return Range(0, end);
}

inline std::ostream &
operator<<(std::ostream & os, const Range & obj)
{
    os << "[" << obj.first() << ", " << obj.last() << ")";
    return os;
}

} // namespace godzilla
