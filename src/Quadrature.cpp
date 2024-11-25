// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Quadrature.h"
#include "godzilla/CallStack.h"
#include "godzilla/Error.h"

namespace godzilla {

Quadrature::Quadrature() : quad(nullptr) {}

Quadrature::Quadrature(PetscQuadrature q) : quad(q) {}

void
Quadrature::create(MPI_Comm comm)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscQuadratureCreate(comm, &this->quad));
}

void
Quadrature::destroy()
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscQuadratureDestroy(&this->quad));
    this->quad = nullptr;
}

void
Quadrature::duplicate(Quadrature & r)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscQuadratureDuplicate(this->quad, &r.quad));
}

Int
Quadrature::get_dim() const
{
    CALL_STACK_MSG();
    Int dim;
    PETSC_CHECK(PetscQuadratureGetData(this->quad, &dim, nullptr, nullptr, nullptr, nullptr));
    return dim;
}

Int
Quadrature::get_num_components() const
{
    CALL_STACK_MSG();
    Int nc;
    PETSC_CHECK(PetscQuadratureGetNumComponents(this->quad, &nc));
    return nc;
}

Int
Quadrature::get_num_points() const
{
    CALL_STACK_MSG();
    Int n_points;
    PETSC_CHECK(PetscQuadratureGetData(this->quad, nullptr, nullptr, &n_points, nullptr, nullptr));
    return n_points;
}

const Real *
Quadrature::get_weights() const
{
    CALL_STACK_MSG();
    const Real * weights;
    PETSC_CHECK(PetscQuadratureGetData(this->quad, nullptr, nullptr, nullptr, nullptr, &weights));
    return weights;
}

Int
Quadrature::get_order() const
{
    CALL_STACK_MSG();
    Int order;
    PETSC_CHECK(PetscQuadratureGetOrder(this->quad, &order));
    return order;
}

bool
Quadrature::equal(const Quadrature & q) const
{
    CALL_STACK_MSG();
    PetscBool eq;
    PETSC_CHECK(PetscQuadratureEqual(this->quad, q.quad, &eq));
    return eq == PETSC_TRUE;
}

Quadrature::operator PetscQuadrature() const
{
    CALL_STACK_MSG();
    return this->quad;
}

Quadrature
Quadrature::create_gauss_tensor(Int dim, Int n_comp, Int n_points, Real a, Real b)
{
    CALL_STACK_MSG();
    Quadrature q;
    PETSC_CHECK(PetscDTGaussTensorQuadrature(dim, n_comp, n_points, a, b, &q.quad));
    return q;
}

} // namespace godzilla
