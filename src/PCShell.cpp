// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/PCShell.h"
#include "godzilla/CallStack.h"
#include "godzilla/Error.h"
#include <petscpc.h>

namespace godzilla {

PetscErrorCode
PCShell::invoke_apply_delegate(PC pc, Vec in_vec, Vec out_vec)
{
    CALL_STACK_MSG();
    PCShell * pcs;
    PETSC_CHECK(PCShellGetContext(pc, &pcs));
    Vector in(in_vec);
    in.inc_reference();
    Vector out(out_vec);
    out.inc_reference();
    pcs->apply_method_.invoke(in, out);
    return 0;
}

PetscErrorCode
PCShell::invoke_apply_ba_delegate(PC pc, PCSide side, Vec in_vec, Vec out_vec, Vec x_vec)
{
    CALL_STACK_MSG();
    PCShell * pcs;
    PETSC_CHECK(PCShellGetContext(pc, &pcs));
    Vector in(in_vec);
    in.inc_reference();
    Vector out(out_vec);
    out.inc_reference();
    Vector x(x_vec);
    x.inc_reference();
    pcs->apply_ba_method_.invoke(side, in, out, x);
    return 0;
}

PetscErrorCode
PCShell::invoke_apply_transpose_delegate(PC pc, Vec in_vec, Vec out_vec)
{
    CALL_STACK_MSG();
    PCShell * pcs;
    PETSC_CHECK(PCShellGetContext(pc, &pcs));
    Vector in(in_vec);
    in.inc_reference();
    Vector out(out_vec);
    out.inc_reference();
    pcs->apply_transpose_method_.invoke(in, out);
    return 0;
}

PetscErrorCode
PCShell::invoke_set_up_delegate(PC pc)
{
    CALL_STACK_MSG();
    PCShell * pcs;
    PETSC_CHECK(PCShellGetContext(pc, &pcs));
    pcs->set_up_method_.invoke();
    return 0;
}

PetscErrorCode
PCShell::invoke_destroy_delegate(PC pc)
{
    CALL_STACK_MSG();
    PCShell * pcs;
    PETSC_CHECK(PCShellGetContext(pc, &pcs));
    pcs->destroy_method_.invoke();
    return 0;
}

PCShell::PCShell() : Preconditioner()
{
    CALL_STACK_MSG();
}

PCShell::PCShell(PC pc) : Preconditioner(pc)
{
    CALL_STACK_MSG();
    Preconditioner::set_type(PCSHELL);
    PETSC_CHECK(PCShellSetContext(pc, this));
}

PCShell::PCShell(const PCShell & other) :
    Preconditioner(other),
    apply_method_(other.apply_method_),
    apply_ba_method_(other.apply_ba_method_),
    apply_transpose_method_(other.apply_transpose_method_),
    set_up_method_(other.set_up_method_),
    destroy_method_(other.destroy_method_)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCShellSetContext(this->obj, this));
}

PCShell &
PCShell::operator=(const PCShell & other)
{
    CALL_STACK_MSG();
    Preconditioner::operator=(other);
    this->apply_method_ = other.apply_method_;
    this->apply_ba_method_ = other.apply_ba_method_;
    this->apply_transpose_method_ = other.apply_transpose_method_;
    this->set_up_method_ = other.set_up_method_;
    this->destroy_method_ = other.destroy_method_;
    PETSC_CHECK(PCShellSetContext(this->obj, this));
    return *this;
}

String
PCShell::get_name() const
{
    CALL_STACK_MSG();
    const char * name;
    PETSC_CHECK(PCShellGetName(this->obj, &name));
    return String(name);
}

void
PCShell::set_name(String name)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCShellSetName(this->obj, name.c_str()));
}

} // namespace godzilla
