// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "godzilla/Assert.h"

namespace godzilla {

template <typename T>
class Array2D {
public:
    /// Create empty array
    Array2D() : n_rows_(-1), n_cols_(-1), data_(nullptr) {}

    /// Create an array with `rows` rows and `cols` columns
    ///
    /// @param rows Number of rows
    /// @param cols Number of columns
    Array2D(Int rows, Int cols) : n_rows_(rows), n_cols_(cols), data_(new T[rows * cols]) {}

    /// Get number of rows
    ///
    /// @return Number of rows
    Int
    get_num_rows()
    {
        return this->n_rows_;
    }

    /// Get number of columns
    ///
    /// @return Number of columns
    Int
    get_num_cols()
    {
        return this->n_cols_;
    }

    /// Allocate memory for the array
    ///
    /// @param rows Number of rows
    /// @param cols Number of columns
    void
    create(Int rows, Int cols)
    {
        this->n_rows_ = rows;
        this->n_cols_ = cols;
        this->data_ = new T[rows * cols];
    }

    /// Set all entiries to zero
    void
    zero()
    {
        GODZILLA_ASSERT_TRUE(this->data != nullptr, "Internal data storage is not allocated");
        for (Int i = 0; i < this->n_rows_ * this->n_cols_; ++i)
            this->data_[i] = 0;
    }

    /// Free memory allocated by the array
    void
    destroy()
    {
        this->n_rows_ = -1;
        this->n_cols_ = -1;
        delete[] this->data_;
        this->data_ = nullptr;
    }

    /// Get entry at specified location for reading
    ///
    /// @param row Row number
    /// @param col Column number
    /// @return Entry at the specified location
    const T &
    get(Int row, Int col) const
    {
        GODZILLA_ASSERT_TRUE(this->data != nullptr, "Internal data storage is not allocated");
        GODZILLA_ASSERT_TRUE((row >= 0) && (row < this->n_rows), "Row index out of bounds");
        GODZILLA_ASSERT_TRUE((col >= 0) && (col < this->n_cols), "Column index out of bounds");
        return this->data_[idx(row, col)];
    }

    /// Assign a value into all entries
    ///
    /// @param val Value to assign
    void
    set(const T & val)
    {
        GODZILLA_ASSERT_TRUE(this->data != nullptr, "Internal data storage is not allocated");
        for (Int i = 0; i < this->n_rows_ * this->n_cols_; ++i)
            this->data_[i] = val;
    }

    /// Get entry at specified location for writing
    ///
    /// @param row Row number
    /// @param col Column number
    /// @return Entry at the specified location
    T &
    set(Int row, Int col)
    {
        GODZILLA_ASSERT_TRUE(this->data != nullptr, "Internal data storage is not allocated");
        GODZILLA_ASSERT_TRUE((row >= 0) && (row < this->n_rows), "Row index out of bounds");
        GODZILLA_ASSERT_TRUE((col >= 0) && (col < this->n_cols), "Column index out of bounds");
        return this->data_[idx(row, col)];
    }

    /// Get an entry from a location for reading
    ///
    /// @param row Row number
    /// @param col Column number
    /// @return Entry at the (row, col) location
    const T &
    operator()(Int row, Int col) const
    {
        return get(row, col);
    }

    /// Get an entry from a location for writing
    ///
    /// @param row Row number
    /// @param col Column number
    /// @return Entry at the (row, col) location
    T &
    operator()(Int row, Int col)
    {
        return set(row, col);
    }

private:
    /// Mapping function from (row, col) to the offset into the internal array that stores the
    /// matrix entries
    ///
    /// @param row Row number
    /// @param col Column number
    /// @return Offset into the `values` array that contains the entry at position (row, col)
    Int
    idx(Int row, Int col) const
    {
        return row * this->n_cols_ + col;
    }

    /// Number of rows
    Int n_rows_;
    /// Number of columns
    Int n_cols_;
    /// Array elements
    T * data_;
};

} // namespace godzilla
