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

    T
    operator[](unsigned int idx) const
    {
        assert(this->data != nullptr);
        return this->data[idx];
    }

protected:
    T * data;
};

/// Used for field values during assembling
class FieldValue : public LateBindArray<Scalar> {};

/// Used for field gradient values during assembling
class FieldGradient : public LateBindArray<Scalar> {
public:
    explicit FieldGradient(const Int & dim) : LateBindArray<Scalar>(), dim(dim) {}

    FieldGradient(const FieldGradient & other) : LateBindArray<Scalar>(other), dim(other.dim) {}

protected:
    const Int & dim;
};

/// Used for vector values during assembling (for example normals)
class Normal : public LateBindArray<Real> {
public:
    explicit Normal(const Int & dim) : LateBindArray<Real>(), dim(dim) {}

    Real
    operator[](unsigned int idx) const
    {
        assert(this->data != nullptr);
        assert(idx < this->dim);
        return this->data[idx];
    }

protected:
    const Int & dim;
};

/// Used for points during assembling (for example physical coordinates)
class Point : public LateBindArray<Real> {
public:
    explicit Point(const Int & dim) : LateBindArray<Real>(), dim(dim) {}

    Real
    operator[](unsigned int idx) const
    {
        assert(this->data != nullptr);
        assert(idx < this->dim);
        return this->data[idx];
    }

protected:
    const Int & dim;
};

} // namespace godzilla
