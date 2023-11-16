#include "godzilla/Functional.h"

namespace godzilla {

Functional::Functional(FEProblemInterface * fepi, const std::string & region) :
    fepi(fepi),
    region(region)
{
}

const std::set<std::string> &
Functional::get_dependent_values() const
{
    _F_;
    return this->depends_on;
}

const std::string &
Functional::get_region() const
{
    _F_;
    return this->region;
}

FEProblemInterface *
Functional::get_fe_problem() const
{
    _F_;
    return this->fepi;
}

const Int &
Functional::get_spatial_dimension() const
{
    _F_;
    return get_fe_problem()->get_spatial_dimension();
}

const Point &
Functional::get_xyz() const
{
    _F_;
    return get_fe_problem()->get_xyz();
}

const FieldValue &
Functional::get_field_value(const std::string & field_name) const
{
    _F_;
    return get_fe_problem()->get_field_value(field_name);
}

const FieldGradient &
Functional::get_field_gradient(const std::string & field_name) const
{
    _F_;
    return get_fe_problem()->get_field_gradient(field_name);
}

const FieldValue &
Functional::get_field_dot(const std::string & field_name) const
{
    _F_;
    return get_fe_problem()->get_field_dot(field_name);
}

const Real &
Functional::get_time() const
{
    _F_;
    return get_fe_problem()->get_assembly_time();
}

} // namespace godzilla
