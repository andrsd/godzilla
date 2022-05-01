#pragma once

namespace godzilla {

/// Dense vector
///
template <typename T>
class DenseVector {
public:
    DenseVector(uint m) : m(m)
    {
        this->data = new T[this->m];
        MEM_CHECK(this->data);
    }

    virtual ~DenseVector() { delete[] this->data; }

    void
    zero()
    {
        for (uint i = 0; i < this->m; i++)
            this->data[i] = 0.;
    }

    T &
    operator[](uint m)
    {
        return this->data[m];
    }

    const T &
    operator[](uint m) const
    {
        return this->data[m];
    }

    /// Number of rows
    const uint &
    rows() const
    {
        return m;
    }

protected:
    /// Number of rows
    uint m;
    /// Vector elements
    T * data;
};

} // namespace godzilla
