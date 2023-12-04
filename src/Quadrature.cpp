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
    _F_;
    PETSC_CHECK(PetscQuadratureCreate(comm, &this->quad));
}

void
Quadrature::destroy()
{
    _F_;
    PETSC_CHECK(PetscQuadratureDestroy(&this->quad));
    this->quad = nullptr;
}

void
Quadrature::duplicate(Quadrature & r)
{
    _F_;
    PETSC_CHECK(PetscQuadratureDuplicate(this->quad, &r.quad));
}

Int
Quadrature::get_dim() const
{
    _F_;
    Int dim;
    PETSC_CHECK(PetscQuadratureGetData(this->quad, &dim, nullptr, nullptr, nullptr, nullptr));
    return dim;
}

Int
Quadrature::get_num_components() const
{
    _F_;
    Int nc;
    PETSC_CHECK(PetscQuadratureGetNumComponents(this->quad, &nc));
    return nc;
}

Int
Quadrature::get_order() const
{
    _F_;
    Int order;
    PETSC_CHECK(PetscQuadratureGetOrder(this->quad, &order));
    return order;
}

bool
Quadrature::equal(const Quadrature & q) const
{
    _F_;
    PetscBool eq;
    PETSC_CHECK(PetscQuadratureEqual(this->quad, q.quad, &eq));
    return eq == PETSC_TRUE;
}

Quadrature::operator PetscQuadrature() const
{
    _F_;
    return this->quad;
}

Quadrature
Quadrature::create_gauss_tensor(Int dim, Int n_comp, Int n_points, Real a, Real b)
{
    _F_;
    Quadrature q;
    PETSC_CHECK(PetscDTGaussTensorQuadrature(dim, n_comp, n_points, a, b, &q.quad));
    return q;
}

} // namespace godzilla
