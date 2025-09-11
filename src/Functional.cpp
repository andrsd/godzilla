// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Functional.h"
#include "godzilla/Types.h"

namespace godzilla {

Functional::Functional(FEProblemInterface * fepi, const std::string & region) :
    fepi(fepi),
    region(region)
{
}

const std::set<std::string> &
Functional::get_dependent_values() const
{
    CALL_STACK_MSG();
    return this->depends_on;
}

const std::string &
Functional::get_region() const
{
    CALL_STACK_MSG();
    return this->region;
}

FEProblemInterface *
Functional::get_fe_problem() const
{
    CALL_STACK_MSG();
    return this->fepi;
}

const Dimension &
Functional::get_spatial_dimension() const
{
    CALL_STACK_MSG();
    return get_fe_problem()->get_spatial_dimension();
}

const Point &
Functional::get_xyz() const
{
    CALL_STACK_MSG();
    return get_fe_problem()->get_xyz();
}

const FieldValue &
Functional::get_field_value(const std::string & field_name) const
{
    CALL_STACK_MSG();
    return get_fe_problem()->get_field_value(field_name);
}

const FieldGradient &
Functional::get_field_gradient(const std::string & field_name) const
{
    CALL_STACK_MSG();
    return get_fe_problem()->get_field_gradient(field_name);
}

const FieldValue &
Functional::get_field_dot(const std::string & field_name) const
{
    CALL_STACK_MSG();
    return get_fe_problem()->get_field_dot(field_name);
}

const Real &
Functional::get_time() const
{
    CALL_STACK_MSG();
    return get_fe_problem()->get_assembly_time();
}

} // namespace godzilla
