#include "BndJacobianFunc.h"
#include "FEProblemInterface.h"
#include "NaturalBC.h"
#include "CallStack.h"

namespace godzilla {

BndJacobianFunc::BndJacobianFunc(const NaturalBC * nbc) :
    Functional(dynamic_cast<const FEProblemInterface *>(nbc->get_discrete_problem_interface()))
{
}

const Int &
BndJacobianFunc::get_spatial_dimension() const
{
    _F_;
    return get_fe_problem()->get_spatial_dimension();
}

const FieldValue &
BndJacobianFunc::get_field_value(const std::string & field_name) const
{
    _F_;
    return get_fe_problem()->get_field_value(field_name);
}

const FieldGradient &
BndJacobianFunc::get_field_gradient(const std::string & field_name) const
{
    _F_;
    return get_fe_problem()->get_field_gradient(field_name);
}

const PetscReal &
BndJacobianFunc::get_time_shift() const
{
    _F_;
    return get_fe_problem()->get_time_shift();
}

const PetscReal &
BndJacobianFunc::get_time() const
{
    _F_;
    return get_fe_problem()->get_time();
}

const Vector &
BndJacobianFunc::get_normal() const
{
    _F_;
    return get_fe_problem()->get_normal();
}

const Point &
BndJacobianFunc::get_xyz() const
{
    _F_;
    return get_fe_problem()->get_xyz();
}

} // namespace godzilla
