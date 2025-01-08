// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Interpolation.h"
#include "petscdm.h"

namespace godzilla {

Interpolation::Interpolation() : info(nullptr) {}

Interpolation::~Interpolation()
{
    if (this->info != nullptr)
        destroy();
}

void
Interpolation::create(MPI_Comm comm)
{
    PETSC_CHECK(DMInterpolationCreate(comm, &this->info));
}

void
Interpolation::destroy()
{
    PETSC_CHECK(DMInterpolationDestroy(&this->info));
    this->info = nullptr;
}

void
Interpolation::add_points(Int n, const Real points[])
{
    auto * data = const_cast<Real *>(points);
    PETSC_CHECK(DMInterpolationAddPoints(this->info, n, data));
}

void
Interpolation::add_points(const std::vector<Real> & points)
{
    auto * data = const_cast<Real *>(points.data());
    PETSC_CHECK(DMInterpolationAddPoints(this->info, points.size(), data));
}

Vector
Interpolation::get_coordinates() const
{
    Vec v;
    PETSC_CHECK(DMInterpolationGetCoordinates(this->info, &v));
    return Vector(v);
}

Int
Interpolation::get_dim() const
{
    Int dim;
    PETSC_CHECK(DMInterpolationGetDim(this->info, &dim));
    return dim;
}

Int
Interpolation::get_dof() const
{
    Int dof;
    PETSC_CHECK(DMInterpolationGetDof(this->info, &dof));
    return dof;
}

Vector
Interpolation::get_vector()
{
    Vec v;
    PETSC_CHECK(DMInterpolationGetVector(this->info, &v));
    return Vector(v);
}

void
Interpolation::restore_vector(Vector & v)
{
    Vec vec = v;
    PETSC_CHECK(DMInterpolationRestoreVector(this->info, &vec));
}

void
Interpolation::set_dim(Int dim)
{
    PETSC_CHECK(DMInterpolationSetDim(this->info, dim));
}

void
Interpolation::set_dof(Int dof)
{
    PETSC_CHECK(DMInterpolationSetDof(this->info, dof));
}

void
Interpolation::set_up(DM dm, bool redundant_points, bool ignore_outside_domain)
{
    PETSC_CHECK(DMInterpolationSetUp(this->info,
                                     dm,
                                     redundant_points ? PETSC_TRUE : PETSC_FALSE,
                                     ignore_outside_domain ? PETSC_TRUE : PETSC_FALSE));
}

void
Interpolation::evaluate(DM dm, const Vector & x, Vector & values)
{
    PETSC_CHECK(DMInterpolationEvaluate(this->info, dm, x, values));
}

} // namespace godzilla
