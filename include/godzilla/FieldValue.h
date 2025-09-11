// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include <cassert>

namespace godzilla {

/// C++ wrapper around a C-array with late binding. Behaves like an array object.
///
/// @tparam T Type of array elements
template <typename T>
class LateBindArray {
public:
    LateBindArray(Int size) : size(size), data(nullptr) {}

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
    operator()(unsigned int idx) const
    {
        assert(this->data != nullptr);
        assert(idx < this->size);
        return this->data[idx];
    }

private:
    /// Number of elements stored in `data`
    Int size;
    /// The elements of the array
    T * data;
};

/// Used for field values during assembling
class FieldValue : public LateBindArray<Scalar> {
public:
    /// Constructor
    ///
    /// @param nc Number of field components
    explicit FieldValue(Int nc) : LateBindArray<Scalar>(nc) {}
};

/// Used for field gradient values during assembling
class FieldGradient : public LateBindArray<Scalar> {
public:
    /// Constructor
    ///
    /// @param dim Spatial dimension
    /// @param nc Number of field components
    explicit FieldGradient(Dimension dim, Int nc) : LateBindArray<Scalar>(nc * dim) {}
};

/// Used for vector values during assembling (for example normals)
class Normal : public LateBindArray<Real> {
public:
    /// Constructor
    ///
    /// @param dim Spatial dimension
    explicit Normal(Dimension dim) : LateBindArray<Real>(dim) {}
};

/// Used for points during assembling (for example physical coordinates)
class Point : public LateBindArray<Real> {
public:
    /// Constructor
    ///
    /// @param dim Spatial dimension
    explicit Point(Dimension dim) : LateBindArray<Real>(dim) {}
};

} // namespace godzilla
