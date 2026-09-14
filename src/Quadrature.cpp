// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Quadrature.h"
#include "godzilla/CallStack.h"
#include "godzilla/Error.h"

namespace godzilla {

Quadrature::Quadrature() : PetscObjectWrapper(nullptr) {}

Quadrature::Quadrature(PetscQuadrature q) : PetscObjectWrapper(q) {}

void
Quadrature::create(mpi::Communicator comm)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscQuadratureCreate(comm, &this->obj_));
}

void
Quadrature::duplicate(Quadrature & r)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscQuadratureDuplicate(this->obj_, &r.obj_));
}

Quadrature
Quadrature::duplicate() const
{
    CALL_STACK_MSG();
    Quadrature q;
    PETSC_CHECK(PetscQuadratureDuplicate(this->obj_, q));
    return q;
}

Dimension
Quadrature::get_dim() const
{
    CALL_STACK_MSG();
    Int dim;
    PETSC_CHECK(PetscQuadratureGetData(this->obj_, &dim, nullptr, nullptr, nullptr, nullptr));
    return Dimension::from_int(dim);
}

Int
Quadrature::get_num_components() const
{
    CALL_STACK_MSG();
    Int nc;
    PETSC_CHECK(PetscQuadratureGetNumComponents(this->obj_, &nc));
    return nc;
}

Int
Quadrature::get_num_points() const
{
    CALL_STACK_MSG();
    Int n_points;
    PETSC_CHECK(PetscQuadratureGetData(this->obj_, nullptr, nullptr, &n_points, nullptr, nullptr));
    return n_points;
}

const Real *
Quadrature::get_weights() const
{
    CALL_STACK_MSG();
    const Real * weights;
    PETSC_CHECK(PetscQuadratureGetData(this->obj_, nullptr, nullptr, nullptr, nullptr, &weights));
    return weights;
}

const Real *
Quadrature::get_points() const
{
    CALL_STACK_MSG();
    const Real * points;
    PETSC_CHECK(PetscQuadratureGetData(this->obj_, nullptr, nullptr, nullptr, &points, nullptr));
    return points;
}

Int
Quadrature::get_order() const
{
    CALL_STACK_MSG();
    Int order;
    PETSC_CHECK(PetscQuadratureGetOrder(this->obj_, &order));
    return order;
}

bool
Quadrature::equal(const Quadrature & q) const
{
    CALL_STACK_MSG();
    PetscBool eq;
    PETSC_CHECK(PetscQuadratureEqual(this->obj_, q.obj_, &eq));
    return eq == PETSC_TRUE;
}

Quadrature
Quadrature::create_gauss_tensor(Dimension dim, Int n_comp, Int n_points, Real a, Real b)
{
    CALL_STACK_MSG();
    Quadrature q;
    PETSC_CHECK(PetscDTGaussTensorQuadrature(dim, n_comp, n_points, a, b, &q.obj_));
    return q;
}

Quadrature
Quadrature::create_simplex(Dimension dim, Int degree, SimplexQuadratureType type)
{
    CALL_STACK_MSG();
    Quadrature q;
    PETSC_CHECK(PetscDTSimplexQuadrature(dim,
                                         degree,
                                         static_cast<PetscDTSimplexQuadratureType>(type),
                                         &q.obj_));
    return q;
}

Quadrature
Quadrature::create_stroud_conical(Dimension dim, Int n_comp, Int n_points, Real a, Real b)
{
    CALL_STACK_MSG();
    Quadrature q;
    PETSC_CHECK(PetscDTStroudConicalQuadrature(dim, n_comp, n_points, a, b, &q.obj_));
    return q;
}

Quadrature
Quadrature::create_tensor_quadrature(Quadrature q1, Quadrature q2)
{
    CALL_STACK_MSG();
    Quadrature q;
    PETSC_CHECK(PetscDTTensorQuadratureCreate(q1, q2, &q.obj_));
    return q;
}

} // namespace godzilla
