#include "BndJacobianFunc.h"
#include "FEProblemInterface.h"
#include "NaturalBC.h"
#include "CallStack.h"

namespace godzilla {

BndJacobianFunc::BndJacobianFunc(const NaturalBC * nbc) :
    fepi(dynamic_cast<const FEProblemInterface *>(nbc->get_discrete_problem_interface()))
{
}

const FEProblemInterface *
BndJacobianFunc::get_fe_problem()
{
    _F_;
    return this->fepi;
}

const PetscInt &
BndJacobianFunc::get_spatial_dimension() const
{
    _F_;
    return this->fepi->get_spatial_dimension();
}

const PetscScalar *
BndJacobianFunc::get_field_value(const std::string & field_name) const
{
    _F_;
    return this->fepi->get_field_value(field_name);
}

const PetscScalar *
BndJacobianFunc::get_field_gradient(const std::string & field_name) const
{
    _F_;
    return this->fepi->get_field_gradient(field_name);
}

const PetscReal &
BndJacobianFunc::get_time_shift() const
{
    _F_;
    return this->fepi->get_time_shift();
}

const PetscReal &
BndJacobianFunc::get_time() const
{
    _F_;
    return this->fepi->get_time();
}

PetscReal * const &
BndJacobianFunc::get_normal() const
{
    _F_;
    return this->fepi->get_normal();
}

PetscReal * const &
BndJacobianFunc::get_xyz() const
{
    _F_;
    return this->fepi->get_xyz();
}

} // namespace godzilla
