// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/VectorScatter.h"
#include "godzilla/Error.h"
#include "godzilla/CallStack.h"
#include "godzilla/IndexSet.h"

namespace godzilla {

VectorScatter::VectorScatter() : PetscObjectWrapper(nullptr) {}

VectorScatter::VectorScatter(VecScatter vs) : PetscObjectWrapper(vs) {}

void
VectorScatter::begin(const Vector & x, Vector & y, InsertMode addv, ScatterMode mode) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecScatterBegin(this->obj, x, y, addv, mode));
}

void
VectorScatter::end(const Vector & x, Vector & y, InsertMode addv, ScatterMode mode) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecScatterEnd(this->obj, x, y, addv, mode));
}

VecScatter
VectorScatter::copy() const
{
    CALL_STACK_MSG();
    VecScatter out;
    PETSC_CHECK(VecScatterCopy(this->obj, &out));
    return out;
}

bool
VectorScatter::get_merged() const
{
    CALL_STACK_MSG();
    PetscBool flg;
    PETSC_CHECK(VecScatterGetMerged(this->obj, &flg));
    return flg == PETSC_TRUE;
}

String
VectorScatter::get_type() const
{
    CALL_STACK_MSG();
    VecScatterType type;
    PETSC_CHECK(VecScatterGetType(this->obj, &type));
    return { type };
}

void
VectorScatter::set_type(VecScatterType type)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecScatterSetType(this->obj, type));
}

void
VectorScatter::set_up()
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecScatterSetUp(this->obj));
}

void
VectorScatter::view(PetscViewer viewer) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecScatterView(this->obj, viewer));
}

VectorScatter
VectorScatter::create(const Vector & x, const IndexSet & ix, const Vector & y, const IndexSet & iy)
{
    CALL_STACK_MSG();
    VecScatter vs;
    PETSC_CHECK(VecScatterCreate(x, ix, y, iy, &vs));
    return VectorScatter(vs);
}

std::tuple<VectorScatter, Vector>
VectorScatter::create_to_all(const Vector & in)
{
    CALL_STACK_MSG();
    VecScatter vs;
    Vec out;
    PETSC_CHECK(VecScatterCreateToAll(in, &vs, &out));
    return { VectorScatter(vs), Vector(out) };
}

std::tuple<VectorScatter, Vector>
VectorScatter::create_to_zero(const Vector & in)
{
    CALL_STACK_MSG();
    VecScatter vs;
    Vec out;
    PETSC_CHECK(VecScatterCreateToZero(in, &vs, &out));
    return { VectorScatter(vs), Vector(out) };
}

} // namespace godzilla
