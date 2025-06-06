// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/NestVector.h"
#include "godzilla/CallStack.h"

namespace godzilla {

NestVector::NestVector() : Vector() {}

NestVector::NestVector(Vec vec) : Vector(vec) {}

Int
NestVector::get_nest_size() const
{
    CALL_STACK_MSG();
    Int n;
    PETSC_CHECK(VecNestGetSize(*this, &n));
    return n;
}

Vector
NestVector::get_sub_vector(Int idx) const
{
    CALL_STACK_MSG();
    Vec sx;
    PETSC_CHECK(VecNestGetSubVec(*this, idx, &sx));
    return Vector(sx);
}

std::vector<Vector>
NestVector::get_sub_vectors() const
{
    CALL_STACK_MSG();
    Int n;
    Vec * sx;
    PETSC_CHECK(VecNestGetSubVecs(*this, &n, &sx));

    std::vector<Vector> vecs(n);
    for (Int i = 0; i < n; ++i)
        vecs[i] = sx[i];
    return vecs;
}

void
NestVector::set_sub_vector(Int idx, Vector sx)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecNestSetSubVec(*this, idx, sx));
}

void
NestVector::set_sub_vectors(const std::vector<Int> & idx, const std::vector<Vector> & sx)
{
    CALL_STACK_MSG();
    assert(idx.size() == sx.size());
    Int n = idx.size();
    std::vector<Vec> vecs(n);
    for (Int i = 0; i < n; ++i)
        vecs[i] = sx[i];
    auto idxm = const_cast<Int *>(idx.data());
    PETSC_CHECK(VecNestSetSubVecs(*this, n, idxm, vecs.data()));
}

} // namespace godzilla
