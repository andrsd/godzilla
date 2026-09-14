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
Preconditioner::create(mpi::Communicator comm)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCCreate(comm, &this->obj_));
}

void
Preconditioner::set_type(String type)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCSetType(this->obj_, type.c_str()));
}

String
Preconditioner::get_type() const
{
    CALL_STACK_MSG();
    PCType type;
    PETSC_CHECK(PCGetType(this->obj_, &type));
    return { type };
}

void
Preconditioner::reset()
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCReset(this->obj_));
}

void
Preconditioner::set_up()
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCSetUp(this->obj_));
}

void
Preconditioner::set_operators(const Matrix & A, const Matrix & P)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCSetOperators(this->obj_, A, P));
}

void
Preconditioner::view(PetscViewer viewer) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCView(this->obj_, viewer));
}

void
Preconditioner::apply(const Vector & x, Vector & y) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCApply(this->obj_, x, y));
}

} // namespace godzilla
