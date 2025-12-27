// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Preconditioner.h"
#include "godzilla/CallStack.h"
#include "godzilla/Error.h"

namespace godzilla {

Preconditioner::Preconditioner() : PetscObjectWrapper(nullptr)
{
    CALL_STACK_MSG();
}

Preconditioner::Preconditioner(PC pc) : PetscObjectWrapper(pc)
{
    CALL_STACK_MSG();
}

void
Preconditioner::create(MPI_Comm comm)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCCreate(comm, &this->obj));
}

void
Preconditioner::destroy()
{
    CALL_STACK_MSG();
}

void
Preconditioner::set_type(String type)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCSetType(this->obj, type.c_str()));
}

String
Preconditioner::get_type() const
{
    CALL_STACK_MSG();
    PCType type;
    PETSC_CHECK(PCGetType(this->obj, &type));
    return { type };
}

void
Preconditioner::reset()
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCReset(this->obj));
}

void
Preconditioner::set_up()
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCSetUp(this->obj));
}

void
Preconditioner::set_operators(const Matrix & A, const Matrix & P)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCSetOperators(this->obj, A, P));
}

void
Preconditioner::view(PetscViewer viewer) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCView(this->obj, viewer));
}

void
Preconditioner::apply(const Vector & x, Vector & y) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCApply(this->obj, x, y));
}

} // namespace godzilla
