// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Quadrature.h"
#include "godzilla/CallStack.h"
#include "godzilla/Error.h"

namespace godzilla {

Quadrature::Quadrature() : PetscObjectWrapper(nullptr) {}

Quadrature::Quadrature(PetscQuadrature q) : PetscObjectWrapper(q) {}

void
Quadrature::create(MPI_Comm comm)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscQuadratureCreate(comm, &this->obj));
}

void
Quadrature::destroy()
{
    CALL_STACK_MSG();
}

void
Quadrature::duplicate(Quadrature & r)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscQuadratureDuplicate(this->obj, &r.obj));
}

Quadrature
Quadrature::duplicate() const
{
    CALL_STACK_MSG();
    Quadrature q;
    PETSC_CHECK(PetscQuadratureDuplicate(this->obj, q));
    return q;
}

Dimension
Quadrature::get_dim() const
{
    CALL_STACK_MSG();
    Int dim;
    PETSC_CHECK(PetscQuadratureGetData(this->obj, &dim, nullptr, nullptr, nullptr, nullptr));
    return Dimension::from_int(dim);
}

Int
Quadrature::get_num_components() const
{
    CALL_STACK_MSG();
    Int nc;
    PETSC_CHECK(PetscQuadratureGetNumComponents(this->obj, &nc));
    return nc;
}

Int
Quadrature::get_num_points() const
{
    CALL_STACK_MSG();
    Int n_points;
    PETSC_CHECK(PetscQuadratureGetData(this->obj, nullptr, nullptr, &n_points, nullptr, nullptr));
    return n_points;
}

const Real *
Quadrature::get_weights() const
{
    CALL_STACK_MSG();
    const Real * weights;
    PETSC_CHECK(PetscQuadratureGetData(this->obj, nullptr, nullptr, nullptr, nullptr, &weights));
    return weights;
}

const Real *
Quadrature::get_points() const
{
    CALL_STACK_MSG();
    const Real * points;
    PETSC_CHECK(PetscQuadratureGetData(this->obj, nullptr, nullptr, nullptr, &points, nullptr));
    return points;
}

Int
Quadrature::get_order() const
{
    CALL_STACK_MSG();
    Int order;
    PETSC_CHECK(PetscQuadratureGetOrder(this->obj, &order));
    return order;
}

bool
Quadrature::equal(const Quadrature & q) const
{
    CALL_STACK_MSG();
    PetscBool eq;
    PETSC_CHECK(PetscQuadratureEqual(this->obj, q.obj, &eq));
    return eq == PETSC_TRUE;
}

Quadrature
Quadrature::create_gauss_tensor(Dimension dim, Int n_comp, Int n_points, Real a, Real b)
{
    CALL_STACK_MSG();
    Quadrature q;
    PETSC_CHECK(PetscDTGaussTensorQuadrature(dim, n_comp, n_points, a, b, &q.obj));
    return q;
}

} // namespace godzilla
