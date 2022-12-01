#include "BndResidualFunc.h"
#include "FEProblemInterface.h"
#include "NaturalBC.h"
#include "CallStack.h"

namespace godzilla {

BndResidualFunc::BndResidualFunc(const NaturalBC * nbc) :
    Functional(dynamic_cast<const FEProblemInterface *>(nbc->get_discrete_problem_interface()))
{
}

const PetscInt &
BndResidualFunc::get_spatial_dimension() const
{
    _F_;
    return get_fe_problem()->get_spatial_dimension();
}

const PetscScalar *
BndResidualFunc::get_field_value(const std::string & field_name) const
{
    _F_;
    return get_fe_problem()->get_field_value(field_name);
}

const PetscScalar *
BndResidualFunc::get_field_gradient(const std::string & field_name) const
{
    _F_;
    return get_fe_problem()->get_field_gradient(field_name);
}

const PetscScalar *
BndResidualFunc::get_field_dot(const std::string & field_name) const
{
    _F_;
    return get_fe_problem()->get_field_dot(field_name);
}

const PetscReal &
BndResidualFunc::get_time() const
{
    _F_;
    return get_fe_problem()->get_time();
}

PetscReal * const &
BndResidualFunc::get_normal() const
{
    _F_;
    return get_fe_problem()->get_normal();
}

PetscReal * const &
BndResidualFunc::get_xyz() const
{
    _F_;
    return get_fe_problem()->get_xyz();
}

} // namespace godzilla
