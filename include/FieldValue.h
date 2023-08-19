#pragma once

#include "petsc.h"
#include <cassert>
#include "Types.h"

namespace godzilla {

/// C++ wrapper around a C-array with late binding. Behaves like an array object.
///
/// @tparam T Type of array elements
template <typename T>
class LateBindArray {
public:
    LateBindArray() : data(nullptr) {}

    void
    set(T * new_data)
    {
        this->data = new_data;
    }

    T *
    get() const
    {
        return this->data;
    }

protected:
    /// The elements of the array
    T * data;
};

/// Used for field values during assembling
class FieldValue : public LateBindArray<Scalar> {
public:
    /// Constructor
    ///
    /// @param nc Number of field components
    explicit FieldValue(Int nc) : LateBindArray<Scalar>() {}

    /// Access an element
    ///
    /// @param c Component number
    /// @return Requested element
    Scalar
    operator()(unsigned int c) const
    {
        assert(this->data != nullptr);
        assert(c < this->nc);
        return this->data[c];
    }

protected:
    /// Number of components
    Int nc;
};

/// Used for field gradient values during assembling
class FieldGradient : public LateBindArray<Scalar> {
public:
    /// Constructor
    ///
    /// @param dim Spatial dimension
    /// @param nc Number of field components
    explicit FieldGradient(Int dim, Int nc) : LateBindArray<Scalar>(), dim(dim), nc(nc) {}

    /// Access an element
    ///
    /// @param idx i-th element of the gradient vector
    /// @param c Component number
    /// @return Requested element
    Scalar
    operator()(unsigned int idx, unsigned int c = 0) const
    {
        assert(this->data != nullptr);
        assert(idx < this->dim);
        assert(c < this->nc);
        return this->data[c * this->dim + idx];
    }

protected:
    /// Spatial dimension
    Int dim;
    /// Number of components
    Int nc;
};

/// Used for vector values during assembling (for example normals)
class Normal : public LateBindArray<Real> {
public:
    /// Constructor
    ///
    /// @param dim Spatial dimension
    explicit Normal(Int dim) : LateBindArray<Real>(), dim(dim) {}

    /// Access a component
    ///
    /// @param idx Component index
    /// @return Requested component
    Real
    operator()(unsigned int idx) const
    {
        assert(this->data != nullptr);
        assert(idx < this->dim);
        return this->data[idx];
    }

protected:
    /// Spatial dimension
    Int dim;
};

/// Used for points during assembling (for example physical coordinates)
class Point : public LateBindArray<Real> {
public:
    /// Constructor
    ///
    /// @param dim Spatial dimension
    explicit Point(Int dim) : LateBindArray<Real>(), dim(dim) {}

    /// Access a component
    ///
    /// @param idx Component index
    /// @return Requested component
    Real
    operator()(unsigned int idx) const
    {
        assert(this->data != nullptr);
        assert(idx < this->dim);
        return this->data[idx];
    }

protected:
    /// Spatial dimension
    Int dim;
};

} // namespace godzilla
