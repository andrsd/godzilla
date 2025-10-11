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
    Array2D() : n_rows(-1), n_cols(-1), data(nullptr) {}

    /// Create an array with `rows` rows and `cols` columns
    ///
    /// @param rows Number of rows
    /// @param cols Number of columns
    Array2D(Int rows, Int cols) : n_rows(rows), n_cols(cols), data(new T[rows * cols]) {}

    /// Get number of rows
    ///
    /// @return Number of rows
    Int
    get_num_rows()
    {
        return this->n_rows;
    }

    /// Get number of columns
    ///
    /// @return Number of columns
    Int
    get_num_cols()
    {
        return this->n_cols;
    }

    /// Allocate memory for the array
    ///
    /// @param rows Number of rows
    /// @param cols Number of columns
    void
    create(Int rows, Int cols)
    {
        this->n_rows = rows;
        this->n_cols = cols;
        this->data = new T[rows * cols];
    }

    /// Set all entiries to zero
    void
    zero()
    {
        assert_true(this->data != nullptr, "Internal data storage is not allocated");
        for (Int i = 0; i < this->n_rows * this->n_cols; ++i)
            this->data[i] = 0;
    }

    /// Free memory allocated by the array
    void
    destroy()
    {
        this->n_rows = -1;
        this->n_cols = -1;
        delete[] this->data;
        this->data = nullptr;
    }

    /// Get entry at specified location for reading
    ///
    /// @param row Row number
    /// @param col Column number
    /// @return Entry at the specified location
    const T &
    get(Int row, Int col) const
    {
        assert_true(this->data != nullptr, "Internal data storage is not allocated");
        assert_true((row >= 0) && (row < this->n_rows), "Row index out of bounds");
        assert_true((col >= 0) && (col < this->n_cols), "Column index out of bounds");
        return this->data[idx(row, col)];
    }

    /// Assign a value into all entries
    ///
    /// @param val Value to assign
    void
    set(const T & val)
    {
        assert_true(this->data != nullptr, "Internal data storage is not allocated");
        for (Int i = 0; i < this->n_rows * this->n_cols; ++i)
            this->data[i] = val;
    }

    /// Get entry at specified location for writing
    ///
    /// @param row Row number
    /// @param col Column number
    /// @return Entry at the specified location
    T &
    set(Int row, Int col)
    {
        assert_true(this->data != nullptr, "Internal data storage is not allocated");
        assert_true((row >= 0) && (row < this->n_rows), "Row index out of bounds");
        assert_true((col >= 0) && (col < this->n_cols), "Column index out of bounds");
        return this->data[idx(row, col)];
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
        return row * this->n_cols + col;
    }

    /// Number of rows
    Int n_rows;
    /// Number of columns
    Int n_cols;
    /// Array elements
    T * data;
};

} // namespace godzilla
