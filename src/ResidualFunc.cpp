#include "ResidualFunc.h"
#include "FEProblemInterface.h"
#include "CallStack.h"

namespace godzilla {

ResidualFunc::ResidualFunc(const FEProblemInterface * fepi) : Functional(fepi) {}

const PetscInt &
ResidualFunc::get_spatial_dimension() const
{
    _F_;
    return get_fe_problem()->get_spatial_dimension();
}

const FieldValue &
ResidualFunc::get_field_value(const std::string & field_name) const
{
    _F_;
    return get_fe_problem()->get_field_value(field_name);
}

const FieldGradient &
ResidualFunc::get_field_gradient(const std::string & field_name) const
{
    _F_;
    return get_fe_problem()->get_field_gradient(field_name);
}

const FieldValue &
ResidualFunc::get_field_dot(const std::string & field_name) const
{
    _F_;
    return get_fe_problem()->get_field_dot(field_name);
}

const PetscReal &
ResidualFunc::get_time() const
{
    _F_;
    return get_fe_problem()->get_time();
}

} // namespace godzilla
