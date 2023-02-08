#include "Godzilla.h"
#include "ConstantIC.h"
#include "CallStack.h"

namespace godzilla {

REGISTER_OBJECT(ConstantIC);

Parameters
ConstantIC::parameters()
{
    Parameters params = InitialCondition::parameters();
    params.add_required_param<std::vector<Real>>("value",
                                                 "Constant values for each field component");
    return params;
}

ConstantIC::ConstantIC(const Parameters & params) :
    InitialCondition(params),
    values(get_param<std::vector<Real>>("value"))
{
    _F_;
}

Int
ConstantIC::get_num_components() const
{
    _F_;
    return (Int) this->values.size();
}

void
ConstantIC::evaluate(Int dim, Real time, const Real x[], Int Nc, Scalar u[])
{
    _F_;
    for (Int i = 0; i < Nc; i++)
        u[i] = this->values[i];
}

} // namespace godzilla
