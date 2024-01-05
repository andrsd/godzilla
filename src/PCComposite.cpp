// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/PCComposite.h"
#include "godzilla/CallStack.h"
#include "godzilla/Error.h"

namespace godzilla {

PCComposite::PCComposite() : Preconditioner()
{
    CALL_STACK_MSG();
}

PCComposite::PCComposite(PC pc) : Preconditioner(pc)
{
    CALL_STACK_MSG();
    Preconditioner::set_type(PCCOMPOSITE);
}

void
PCComposite::create(MPI_Comm comm)
{
    CALL_STACK_MSG();
    Preconditioner::create(comm);
    Preconditioner::set_type(PCCOMPOSITE);
}

void
PCComposite::set_type(Type type)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCCompositeSetType(this->pc, static_cast<PCCompositeType>(type)));
}

PCComposite::Type
PCComposite::get_type()
{
    CALL_STACK_MSG();
    PCCompositeType type;
    PETSC_CHECK(PCCompositeGetType(this->pc, &type));
    return static_cast<Type>(type);
}

void
PCComposite::add_pc(const Preconditioner & subpc)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCCompositeAddPC(this->pc, subpc));
}

Int
PCComposite::get_number_pc() const
{
    CALL_STACK_MSG();
    Int num;
    PETSC_CHECK(PCCompositeGetNumberPC(this->pc, &num));
    return num;
}

Preconditioner
PCComposite::get_pc(Int n) const
{
    CALL_STACK_MSG();
    PC subpc;
    PETSC_CHECK(PCCompositeGetPC(this->pc, n, &subpc));
    return Preconditioner(subpc);
}

void
PCComposite::special_set_alpha(Scalar alpha)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCCompositeSpecialSetAlpha(this->pc, alpha));
}

} // namespace godzilla
