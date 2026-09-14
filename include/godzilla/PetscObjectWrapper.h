// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Error.h"
#include "godzilla/CallStack.h"
#include "godzilla/Types.h"
#include "godzilla/Assert.h"
#include "mpicpp-lite/mpicpp-lite.h"
#include <type_traits>

namespace mpi = mpicpp_lite;

namespace godzilla {

// Base template for PETSc objects with reference counting
template <typename T>
class PetscObjectWrapper {
    static_assert(std::is_pointer_v<T>, "T must be a PETSc pointer type (e.g., Vec, Mat, KSP)");

protected:
    T obj_ = nullptr;

    explicit PetscObjectWrapper(T obj) : obj_(obj) { CALL_STACK_MSG(); }

public:
    virtual ~PetscObjectWrapper()
    {
        CALL_STACK_MSG();
        release();
    }

    // Copy constructor: increment reference
    PetscObjectWrapper(const PetscObjectWrapper & other) : obj_(other.obj_)
    {
        CALL_STACK_MSG();
        inc_reference();
    }

    // Copy assignment
    PetscObjectWrapper &
    operator=(const PetscObjectWrapper & other)
    {
        CALL_STACK_MSG();
        if (this != &other) {
            release();
            this->obj_ = other.obj_;
            if (this->obj_)
                inc_reference();
        }
        return *this;
    }

    // Move constructor
    PetscObjectWrapper(PetscObjectWrapper && other) noexcept : obj_(other.obj_)
    {
        CALL_STACK_MSG();
        other.obj_ = nullptr;
    }

    // Move assignment
    PetscObjectWrapper &
    operator=(PetscObjectWrapper && other) noexcept
    {
        CALL_STACK_MSG();
        if (this != &other) {
            release();
            this->obj_ = other.obj_;
            other.obj_ = nullptr;
        }
        return *this;
    }

    mpi::Communicator
    get_comm() const
    {
        MPI_Comm comm;
        PETSC_CHECK(PetscObjectGetComm((PetscObject) this->obj_, &comm));
        return { comm };
    }

    T
    get() const noexcept
    {
        CALL_STACK_MSG();
        return this->obj_;
    }

    explicit
    operator bool() const noexcept
    {
        CALL_STACK_MSG();
        return !is_null();
    }

    explicit
    operator bool() noexcept
    {
        CALL_STACK_MSG();
        return !is_null();
    }

    /// Returns the PETSc ID of the index set
    ///
    /// @return The PETSc ID
    PetscObjectId
    get_id() const
    {
        CALL_STACK_MSG();
        GODZILLA_ASSERT_TRUE(this->obj != nullptr, "PETSc object is null");
        PetscObjectId id;
        PETSC_CHECK(PetscObjectGetId((PetscObject) this->obj_, &id));
        return id;
    }

    /// Sets a string name for a PETSc object
    ///
    /// @param name Object name to set
    void
    set_name(String name)
    {
        CALL_STACK_MSG();
        PETSC_CHECK(PetscObjectSetName((PetscObject) this->obj_, name.c_str()));
    }

    /// Gets a string name associated with a PETSc object.
    ///
    /// @return The name associated with object
    String
    get_name()
    {
        CALL_STACK_MSG();
        if (this->obj_) {
            const char * name = nullptr;
            PETSC_CHECK(PetscObjectGetName((PetscObject) this->obj_, &name));
            return { name };
        }
        else
            return { "" };
    }

    bool
    is_null() const
    {
        CALL_STACK_MSG();
        return this->obj_ == nullptr;
    }

    /// Get reference count
    ///
    /// @return Reference count
    Int
    reference_count() const
    {
        CALL_STACK_MSG();
        Int count = 0;
        if (this->obj_)
            PETSC_CHECK(PetscObjectGetReference(reinterpret_cast<PetscObject>(this->obj_), &count));
        return count;
    }

    /// Convert this object to a PETSc object so it can be passed directly into PETSc API
    operator T() const
    {
        CALL_STACK_MSG();
        return this->obj_;
    }

    /// Convert this object to a PETSc object so it can be passed directly into PETSc API
    operator T *()
    {
        CALL_STACK_MSG();
        return &this->obj_;
    }

    bool
    operator==(const PetscObjectWrapper & other) const noexcept
    {
        CALL_STACK_MSG();
        return this->obj_ == other.obj_;
    }

    bool
    operator!=(const PetscObjectWrapper & other) const noexcept
    {
        CALL_STACK_MSG();
        return this->obj_ != other.obj_;
    }

    /// Increase the reference count to this object
    void
    inc_reference()
    {
        CALL_STACK_MSG();
        PETSC_CHECK(PetscObjectReference(reinterpret_cast<PetscObject>(this->obj_)));
    }

private:
    void
    release() noexcept
    {
        CALL_STACK_MSG();
        if (this->obj_)
            PETSC_CHECK(PetscObjectDereference(reinterpret_cast<PetscObject>(this->obj_)));
    }

    friend class Mesh;
    friend class UnstructuredMesh;
    friend class NonlinearProblem;
    friend class LinearProblem;
};

} // namespace godzilla
