#pragma once

#include "petsc.h"
#include <cassert>

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
class FieldValue : public LateBindArray<PetscScalar> {};

/// Used for field gradient values during assembling
class FieldGradient : public LateBindArray<PetscScalar> {
public:
    explicit FieldGradient(const PetscInt & dim) : LateBindArray<PetscScalar>(), dim(dim) {}

    FieldGradient(const FieldGradient & other) : LateBindArray<PetscScalar>(other), dim(other.dim)
    {
    }

protected:
    const PetscInt & dim;
};

/// Used for vector values during assembling (for example normals)
class Vector : public LateBindArray<PetscReal> {
public:
    explicit Vector(const PetscInt & dim) : LateBindArray<PetscReal>(), dim(dim) {}

    PetscReal
    operator[](unsigned int idx) const
    {
        assert(this->data != nullptr);
        assert(idx < this->dim);
        return this->data[idx];
    }

protected:
    const PetscInt & dim;
};

/// Used for points during assembling (for example physical coordinates)
class Point : public LateBindArray<PetscReal> {
public:
    explicit Point(const PetscInt & dim) : LateBindArray<PetscReal>(), dim(dim) {}

    PetscReal
    operator[](unsigned int idx) const
    {
        assert(this->data != nullptr);
        assert(idx < this->dim);
        return this->data[idx];
    }

protected:
    const PetscInt & dim;
};

} // namespace godzilla
