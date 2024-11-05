// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Preconditioner.h"
#include "godzilla/CallStack.h"
#include "godzilla/Error.h"

namespace godzilla {

Preconditioner::Preconditioner() : pc(nullptr)
{
    CALL_STACK_MSG();
}

Preconditioner::Preconditioner(PC pc) : pc(pc)
{
    CALL_STACK_MSG();
}

void
Preconditioner::create(MPI_Comm comm)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCCreate(comm, &this->pc));
}

void
Preconditioner::destroy()
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCDestroy(&this->pc));
    this->pc = nullptr;
}

void
Preconditioner::set_type(const std::string & type)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCSetType(this->pc, type.c_str()));
}

std::string
Preconditioner::get_type() const
{
    CALL_STACK_MSG();
    PCType type;
    PETSC_CHECK(PCGetType(this->pc, &type));
    return { type };
}

void
Preconditioner::reset()
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCReset(this->pc));
}

void
Preconditioner::set_up()
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCSetUp(this->pc));
}

void
Preconditioner::set_operators(const Matrix & A, const Matrix & P)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCSetOperators(this->pc, A, P));
}

void
Preconditioner::view(PetscViewer viewer) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCView(this->pc, viewer));
}

Preconditioner::operator PC() const
{
    CALL_STACK_MSG();
    return this->pc;
}

void
Preconditioner::apply(const Vector & x, Vector & y) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCApply(this->pc, x, y));
}

} // namespace godzilla
