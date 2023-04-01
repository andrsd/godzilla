#include "BndResidualFunc.h"
#include "FEProblemInterface.h"
#include "NaturalBC.h"
#include "CallStack.h"

namespace godzilla {

BndResidualFunc::BndResidualFunc(const NaturalBC * nbc) :
    ResidualFunc(dynamic_cast<const FEProblemInterface *>(nbc->get_discrete_problem_interface()),
                 nbc->get_boundary())
{
}

const FieldValue &
BndResidualFunc::get_field_value(const std::string & field_name) const
{
    _F_;
    auto nm = get_value_name(field_name);
    return get_fe_problem()->get_field_value(nm);
}

const FieldGradient &
BndResidualFunc::get_field_gradient(const std::string & field_name) const
{
    _F_;
    auto nm = get_value_name(field_name);
    return get_fe_problem()->get_field_gradient(nm);
}

const FieldValue &
BndResidualFunc::get_field_dot(const std::string & field_name) const
{
    _F_;
    auto nm = get_value_name(field_name);
    return get_fe_problem()->get_field_dot(nm);
}

const Normal &
BndResidualFunc::get_normal() const
{
    _F_;
    return get_fe_problem()->get_normal();
}

} // namespace godzilla
