// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/PCShell.h"
#include "godzilla/CallStack.h"
#include "godzilla/Error.h"
#include <petscpc.h>

namespace godzilla {

ErrorCode
PCShell::invoke_apply_delegate(PC pc, Vec in_vec, Vec out_vec)
{
    CALL_STACK_MSG();
    PCShell * pcs;
    PETSC_CHECK(PCShellGetContext(pc, &pcs));
    Vector in(in_vec);
    in.inc_reference();
    Vector out(out_vec);
    out.inc_reference();
    pcs->apply_method.invoke(in, out);
    return 0;
}

ErrorCode
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
    pcs->apply_ba_method.invoke(side, in, out, x);
    return 0;
}

ErrorCode
PCShell::invoke_apply_transpose_delegate(PC pc, Vec in_vec, Vec out_vec)
{
    CALL_STACK_MSG();
    PCShell * pcs;
    PETSC_CHECK(PCShellGetContext(pc, &pcs));
    Vector in(in_vec);
    in.inc_reference();
    Vector out(out_vec);
    out.inc_reference();
    pcs->apply_transpose_method.invoke(in, out);
    return 0;
}

ErrorCode
PCShell::invoke_set_up_delegate(PC pc)
{
    CALL_STACK_MSG();
    PCShell * pcs;
    PETSC_CHECK(PCShellGetContext(pc, &pcs));
    pcs->set_up_method.invoke();
    return 0;
}

ErrorCode
PCShell::invoke_destroy_delegate(PC pc)
{
    CALL_STACK_MSG();
    PCShell * pcs;
    PETSC_CHECK(PCShellGetContext(pc, &pcs));
    pcs->destroy_method.invoke();
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
    apply_method(other.apply_method),
    apply_ba_method(other.apply_ba_method),
    apply_transpose_method(other.apply_transpose_method),
    set_up_method(other.set_up_method),
    destroy_method(other.destroy_method)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCShellSetContext(this->obj, this));
}

PCShell &
PCShell::operator=(const PCShell & other)
{
    CALL_STACK_MSG();
    Preconditioner::operator=(other);
    this->apply_method = other.apply_method;
    this->apply_ba_method = other.apply_ba_method;
    this->apply_transpose_method = other.apply_transpose_method;
    this->set_up_method = other.set_up_method;
    this->destroy_method = other.destroy_method;
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
PCShell::set_name(const String & name)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCShellSetName(this->obj, name.c_str()));
}

} // namespace godzilla
