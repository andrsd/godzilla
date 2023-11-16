#include "godzilla/Godzilla.h"
#include "godzilla/ConstantInitialCondition.h"
#include "godzilla/CallStack.h"

namespace godzilla {

REGISTER_OBJECT(ConstantInitialCondition);

Parameters
ConstantInitialCondition::parameters()
{
    Parameters params = InitialCondition::parameters();
    params.add_required_param<std::vector<Real>>("value",
                                                 "Constant values for each field component");
    return params;
}

ConstantInitialCondition::ConstantInitialCondition(const Parameters & params) :
    InitialCondition(params),
    values(get_param<std::vector<Real>>("value"))
{
    _F_;
}

Int
ConstantInitialCondition::get_num_components() const
{
    _F_;
    return (Int) this->values.size();
}

void
ConstantInitialCondition::evaluate(Int dim, Real time, const Real x[], Int Nc, Scalar u[])
{
    _F_;
    for (Int i = 0; i < Nc; i++)
        u[i] = this->values[i];
}

} // namespace godzilla
