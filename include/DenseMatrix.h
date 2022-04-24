#pragma once

#include "Error.h"
#include <assert.h>

namespace godzilla {

/// Dense matrix
///
template <typename T>
class DenseMatrix {
public:
    /// Creates a matrix with `m` rows and `n` columns with entries of the type T.
    /// The entries can be accessed by matrix[i][j].
    DenseMatrix(uint m, uint n = 0) : m(m), n(n == 0 ? m : n)
    {
        this->data = new T *[this->m];
        MEM_CHECK(this->data);
        for (uint i = 0; i < this->m; i++) {
            this->data[i] = new T[this->n];
            MEM_CHECK(this->data[i]);
        }
    }

    ~DenseMatrix()
    {
        for (uint i = 0; i < this->m; i++)
            delete[] this->data[i];
        delete[] this->data;
    }

    void
    zero()
    {
        for (uint i = 0; i < this->m; i++)
            for (uint j = 0; j < this->n; j++)
                this->data[i][j] = 0.;
    }

    /// Transposes an m by n matrix. If m != n, the array matrix in fact has to be
    /// a square matrix of the size max(m, n) in order for the transpose to fit inside it.
    void
    transpose()
    {
        assert(this->m == this->n);
        for (uint i = 0; i < this->m; i++)
            for (uint j = i + 1; j < this->n; j++)
                std::swap(this->data[i][j], this->data[j][i]);
    }

    /// Changes the sign of a matrix
    void
    chsgn()
    {
        for (uint i = 0; i < this->m; i++)
            for (uint j = 0; j < this->n; j++)
                this->data[i][j] = -this->data[i][j];
    }

    T *
    operator[](uint m)
    {
        return this->data[m];
    }

    const T *
    operator[](uint m) const
    {
        return this->data[m];
    }

    const uint & rows() const { return m; }

    const uint & cols() const { return n; }

protected:
    /// Number of rows
    uint m;
    /// Number of columns
    uint n;
    /// Matrix elements
    T ** data;
};

} // namespace godzilla
