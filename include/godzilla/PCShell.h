// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Preconditioner.h"
#include <petscpc.h>

namespace godzilla {

/// Preconditioner class for use with a users own private data storage format and preconditioner
/// application code
class PCShell : public Preconditioner {
public:
    PCShell();
    explicit PCShell(PC pc);

    PCShell(const PCShell & other);

    /// Get the name of the preconditioner
    ///
    /// @return The name of the preconditioner
    std::string get_name() const;

    /// Set the name of the preconditioner
    ///
    /// @param name The name of the preconditioner
    void set_name(const std::string & name);

    /// Set the apply method
    template <class T>
    void
    set_apply(T * instance, void (T::*method)(const Vector &, Vector &))
    {
        this->apply_method.bind(instance, method);
        PETSC_CHECK(PCShellSetApply(*this, invoke_apply_delegate));
    }

    /// Set the apply BA method
    template <class T>
    void
    set_apply_ba(T * instance, void (T::*method)(PCSide, const Vector &, Vector &, Vector &))
    {
        this->apply_ba_method.bind(instance, method);
        PETSC_CHECK(PCShellSetApplyBA(*this, invoke_apply_ba_delegate));
    }

    /// Set the apply transpose method
    template <class T>
    void
    set_apply_transpose(T * instance, void (T::*method)(const Vector &, Vector &))
    {
        this->apply_transpose_method.bind(instance, method);
        PETSC_CHECK(PCShellSetApplyTranspose(*this, invoke_apply_transpose_delegate));
    }

    /// Set the set up method
    template <class T>
    void
    set_set_up(T * instance, void (T::*method)(void))
    {
        this->set_up_method.bind(instance, method);
        PETSC_CHECK(PCShellSetSetUp(*this, invoke_set_up_delegate));
    }

    /// Set the destroy method
    template <class T>
    void
    set_destroy(T * instance, void (T::*method)(void))
    {
        this->destroy_method.bind(instance, method);
        PETSC_CHECK(PCShellSetDestroy(*this, invoke_destroy_delegate));
    }

    PCShell & operator=(const PCShell & other);

private:
    /// Method for the apply function
    Delegate<void(const Vector &, Vector &)> apply_method;
    /// Method for the apply_ba function
    Delegate<void(PCSide, const Vector &, Vector &, Vector &)> apply_ba_method;
    /// Method for the apply_transpose function
    Delegate<void(const Vector &, Vector &)> apply_transpose_method;
    /// Method for the set_up function
    Delegate<void(void)> set_up_method;
    /// Method for the destroy function
    Delegate<void(void)> destroy_method;

public:
    static ErrorCode invoke_apply_delegate(PC, Vec, Vec);
    static ErrorCode invoke_apply_ba_delegate(PC, PCSide, Vec, Vec, Vec);
    static ErrorCode invoke_apply_transpose_delegate(PC, Vec, Vec);
    static ErrorCode invoke_set_up_delegate(PC);
    static ErrorCode invoke_destroy_delegate(PC);
};

} // namespace godzilla
