// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/PCJacobi.h"
#include "godzilla/CallStack.h"
#include "godzilla/Error.h"

namespace godzilla {

PCJacobi::PCJacobi() : Preconditioner()
{
    CALL_STACK_MSG();
}

PCJacobi::PCJacobi(PC pc) : Preconditioner(pc)
{
    CALL_STACK_MSG();
    Preconditioner::set_type(PCJACOBI);
}

void
PCJacobi::create(MPI_Comm comm)
{
    CALL_STACK_MSG();
    Preconditioner::create(comm);
    Preconditioner::set_type(PCJACOBI);
}

void
PCJacobi::set_type(Type type)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCJacobiSetType(this->obj, static_cast<PCJacobiType>(type)));
}

PCJacobi::Type
PCJacobi::get_type() const
{
    CALL_STACK_MSG();
    PCJacobiType type;
    PETSC_CHECK(PCJacobiGetType(this->obj, &type));
    return static_cast<Type>(type);
}

bool
PCJacobi::get_fix_diagonal() const
{
    CALL_STACK_MSG();
    PetscBool flag;
    PETSC_CHECK(PCJacobiGetFixDiagonal(this->obj, &flag));
    return flag == PETSC_TRUE;
}

bool
PCJacobi::get_use_abs() const
{
    CALL_STACK_MSG();
    PetscBool flag;
    PETSC_CHECK(PCJacobiGetUseAbs(this->obj, &flag));
    return flag == PETSC_TRUE;
}

void
PCJacobi::set_fix_diagonal(bool flag)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCJacobiSetFixDiagonal(this->obj, flag ? PETSC_TRUE : PETSC_FALSE));
}

void
PCJacobi::set_use_abs(bool flag)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCJacobiSetUseAbs(this->obj, flag ? PETSC_TRUE : PETSC_FALSE));
}

} // namespace godzilla
