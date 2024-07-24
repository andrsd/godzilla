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

        explicit Iterator(Int idx) : idx(idx) {}

        const value_type &
        operator*() const
        {
            return this->idx;
        }

        /// Prefix increment
        Iterator &
        operator++()
        {
            this->idx++;
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

        friend bool
        operator==(const Iterator & a, const Iterator & b)
        {
            return a.idx == b.idx;
        };

        friend bool
        operator!=(const Iterator & a, const Iterator & b)
        {
            return a.idx != b.idx;
        };

    private:
        Int idx;
    };

    Range() : first_idx(-1), last_idx(-1) {}
    Range(Int first, Int last) : first_idx(first), last_idx(last) {}

    [[nodiscard]] Iterator
    begin() const
    {
        return Iterator(this->first_idx);
    }

    [[nodiscard]] Iterator
    end() const
    {
        return Iterator(this->last_idx);
    }

    /// Get the number of indices in the range
    [[nodiscard]] Int
    size() const
    {
        return last_idx - first_idx;
    }

    /// Get the first index in the range
    ///
    /// @return First index in the range
    [[nodiscard]] Int
    first() const
    {
        return first_idx;
    }

    /// Get the last index (not included) in the range
    ///
    /// @return Last index (not included) in the range
    [[nodiscard]] Int
    last() const
    {
        return last_idx;
    }

private:
    /// First index
    Int first_idx;
    /// Last index (not included in the range)
    Int last_idx;
};

} // namespace godzilla
