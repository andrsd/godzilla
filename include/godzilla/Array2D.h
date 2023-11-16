#pragma once

#include "godzilla/Types.h"
#include "godzilla/Error.h"
#include <cassert>

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
    Array2D(Int rows, Int cols) : n_rows(rows), n_cols(cols), data(new T[rows * cols])
    {
        MEM_CHECK(this->data);
    }

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
        MEM_CHECK(this->data);
    }

    /// Set all entiries to zero
    void
    zero()
    {
        assert(this->data != nullptr);
        for (Int i = 0; i < this->n_rows * this->n_cols; i++)
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

    /// Get an entry from a location for reading
    ///
    /// @param row Row number
    /// @param col Column number
    /// @return Entry at the (row, col) location
    const T &
    operator()(Int row, Int col) const
    {
        assert(this->data != nullptr);
        assert((row >= 0) && (row < this->n_rows));
        assert((col >= 0) && (col < this->n_cols));
        return this->data[col * this->n_rows + row];
    }

    /// Get an entry from a location for writing
    ///
    /// @param row Row number
    /// @param col Column number
    /// @return Entry at the (row, col) location
    T &
    operator()(Int row, Int col)
    {
        assert(this->data != nullptr);
        assert((row >= 0) && (row < this->n_rows));
        assert((col >= 0) && (col < this->n_cols));
        return this->data[col * this->n_rows + row];
    }

private:
    /// Number of rows
    Int n_rows;
    /// Number of columns
    Int n_cols;
    /// Array elements
    T * data;
};

} // namespace godzilla
