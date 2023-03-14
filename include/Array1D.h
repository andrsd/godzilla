#pragma once

#include "Types.h"
#include "Error.h"
#include <cassert>

namespace godzilla {

template <typename T, Int N>
class DenseVector;

template <typename T>
class Array1D {
public:
    /// Create an empty array
    Array1D() : n(-1), data(nullptr) {}

    /// Create an array with specified number of entries
    ///
    /// @param size Number of entries in the array
    explicit Array1D(Int size) : n(size), data(new T[size]) { MEM_CHECK(this->data); }

    /// Allocate memory for the array with specified number of entries
    ///
    /// @param size Number of entries in the array
    void
    create(Int size)
    {
        this->n = size;
        this->data = new T[size];
        MEM_CHECK(this->data);
    }

    /// Get number of entries in the array
    ///
    /// @return Number of entries in the array
    Int
    get_size() const
    {
        return this->n;
    }

    /// Get the entry at a specified location for reading
    ///
    /// @param i Index fo the entry
    /// @return Entry at the `ith` location
    const T &
    get(Int i) const
    {
        assert(this->data != nullptr);
        assert((i >= 0) && (i < this->n));
        return this->data[i];
    }

    /// Get the entry at a specified location for writing
    ///
    /// @param i Index fo the entry
    /// @return Entry at the `ith` location
    T &
    set(Int i)
    {
        assert(this->data != nullptr);
        assert((i >= 0) && (i < this->n));
        return this->data[i];
    }

    /// Set all entries in the array to zero
    void
    zero()
    {
        assert(this->data != nullptr);
        for (Int i = 0; i < this->n; i++)
            this->data[i].zero();
    }

    /// Free memory allocated by this array
    void
    destroy()
    {
        delete[] this->data;
        this->data = nullptr;
        this->n = -1;
    }

    /// Get values from specified indices
    ///
    /// @tparam N Size of the index array
    /// @param idx Indices to get the values from
    /// @return Vector with the value from locations specified by `idx`
    template <Int N>
    DenseVector<T, N>
    get_values(DenseVector<Int, N> idx) const
    {
        DenseVector<T, N> res;
        for (Int i = 0; i < N; i++)
            res(i) = get(idx(i));
        return res;
    }

    /// Assign a value into all vector entries, i.e. `vec[i] = val`
    ///
    /// @param val Value to assign
    void
    set_values(const T & val)
    {
        assert(this->data != nullptr);
        for (Int i = 0; i < this->n; i++)
            this->data[i] = val;
    }

    /// Set multiple values into the vector
    ///
    /// @param vals Values to set into this vector
    void
    set_values(const std::vector<T> & vals)
    {
        assert(this->data != nullptr);
        assert(this->n == vals.size());
        for (Int i = 0; i < this->n; i++)
            this->data[i] = vals[i];
    }

    /// Set multiple values at specified indices
    ///
    /// @tparam N Size of the array
    /// @param idx Indices where the values are to be set
    /// @param a Values to be set
    template <Int N>
    void
    set_values(const DenseVector<Int, N> & idx, const DenseVector<T, N> & a)
    {
        for (Int i = 0; i < N; i++)
            set(idx(i)) = a(i);
    }

    /// Multiply all entries in the array by a value
    ///
    /// @param alpha Value to multiply all entries with
    void
    scale(Real alpha) const
    {
        for (Int i = 0; i < this->n; i++)
            this->data[i] *= alpha;
    }

    /// Add values to specified locations
    ///
    /// @tparam N Size of the index and value array
    /// @param idx Indices to modify
    /// @param a Vector with values to add at locations specified by `idx`
    template <Int N>
    void
    add(const DenseVector<Int, N> & idx, const DenseVector<T, N> & a)
    {
        for (Int i = 0; i < N; i++)
            set(idx(i)) += a(i);
    }

    /// Add alpha * x to this vector
    ///
    /// @param alpha Scalar value
    /// @param x Vector to add
    void
    axpy(Real alpha, const Array1D<T> & x) const
    {
        error("AXPY not implemented");
    }

    /// Compute dot product of (this * `a`)
    ///
    /// @param a Vector to multiply with
    /// @return Dot product
    T
    dot(const Array1D<T> & a) const
    {
        error("Dot product not implemented");
    }

    // operators

    const T &
    operator()(Int i) const
    {
        return get(i);
    }

    T &
    operator()(Int i)
    {
        return set(i);
    }

    T
    operator*(const Array1D<T> & a) const
    {
        return dot(a);
    }

    //

    // Do your best to avoid abusing this API
    T *
    get_data() const
    {
        return this->data;
    }

private:
    /// Number of entries in the array
    Int n;
    /// Array containing the values
    T * data;
};

template <>
inline void
Array1D<Real>::axpy(Real alpha, const Array1D<Real> & x) const
{
    for (Int i = 0; i < this->n; i++)
        this->data[i] = alpha * x(i) + this->data[i];
}

template <>
inline Real
Array1D<Real>::dot(const Array1D<Real> & a) const
{
    Real res = 0;
    for (Int i = 0; i < this->n; i++)
        res += get(i) * a(i);
    return res;
}

template <>
inline void
Array1D<Real>::zero()
{
    assert(this->data != nullptr);
    for (Int i = 0; i < this->n; i++)
        this->data[i] = 0.;
}

// Output

template <typename T>
std::ostream &
operator<<(std::ostream & os, const Array1D<T> & obj)
{
    os << "(";
    for (Int i = 0; i < obj.get_size(); i++) {
        os << obj(i);
        if (i < obj.get_size() - 1)
            os << ", ";
    }
    os << ")";
    return os;
}

} // namespace godzilla
