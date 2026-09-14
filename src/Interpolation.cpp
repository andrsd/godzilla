// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Interpolation.h"
#include "petscdm.h"

namespace godzilla {

Interpolation::Interpolation() : info_(nullptr) {}

void
Interpolation::create(mpi::Communicator comm)
{
    PETSC_CHECK(DMInterpolationCreate(comm, &this->info_));
}

void
Interpolation::destroy()
{
    PETSC_CHECK(DMInterpolationDestroy(&this->info_));
}

void
Interpolation::add_points(Int n, const Real points[])
{
    auto * data = const_cast<Real *>(points);
    PETSC_CHECK(DMInterpolationAddPoints(this->info_, n, data));
}

void
Interpolation::add_points(std::initializer_list<Real> points)
{
    PETSC_CHECK(DMInterpolationAddPoints(this->info_,
                                         points.size(),
                                         const_cast<Real *>(std::data(points))));
}

void
Interpolation::add_points(Span<Real> points)
{
    PETSC_CHECK(DMInterpolationAddPoints(this->info_, points.size(), points.data()));
}

Vector
Interpolation::get_coordinates() const
{
    Vector v;
    PETSC_CHECK(DMInterpolationGetCoordinates(this->info_, v));
    v.inc_reference();
    return v;
}

Dimension
Interpolation::get_dim() const
{
    Int dim;
    PETSC_CHECK(DMInterpolationGetDim(this->info_, &dim));
    return Dimension::from_int(dim);
}

Int
Interpolation::get_dof() const
{
    Int dof;
    PETSC_CHECK(DMInterpolationGetDof(this->info_, &dof));
    return dof;
}

Vector
Interpolation::get_vector()
{
    Vector v;
    PETSC_CHECK(DMInterpolationGetVector(this->info_, v));
    v.inc_reference();
    return v;
}

void
Interpolation::restore_vector(Vector & v)
{
    Vec vec = v;
    PETSC_CHECK(DMInterpolationRestoreVector(this->info_, &vec));
}

void
Interpolation::set_dim(Dimension dim)
{
    PETSC_CHECK(DMInterpolationSetDim(this->info_, dim));
}

void
Interpolation::set_dof(Int dof)
{
    PETSC_CHECK(DMInterpolationSetDof(this->info_, dof));
}

void
Interpolation::set_up(DM dm, bool redundant_points, bool ignore_outside_domain)
{
    PETSC_CHECK(DMInterpolationSetUp(this->info_,
                                     dm,
                                     redundant_points ? PETSC_TRUE : PETSC_FALSE,
                                     ignore_outside_domain ? PETSC_TRUE : PETSC_FALSE));
}

void
Interpolation::evaluate(DM dm, const Vector & x, Vector & values)
{
    PETSC_CHECK(DMInterpolationEvaluate(this->info_, dm, x, values));
}

} // namespace godzilla
