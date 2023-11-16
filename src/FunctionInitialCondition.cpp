#include "godzilla/Godzilla.h"
#include "godzilla/CallStack.h"
#include "godzilla/FunctionInitialCondition.h"

namespace godzilla {

REGISTER_OBJECT(FunctionInitialCondition);

Parameters
FunctionInitialCondition::parameters()
{
    Parameters params = InitialCondition::parameters();
    params += FunctionInterface::parameters();
    return params;
}

FunctionInitialCondition::FunctionInitialCondition(const Parameters & params) :
    InitialCondition(params),
    FunctionInterface(params)
{
}

void
FunctionInitialCondition::create()
{
    _F_;
    InitialCondition::create();
    FunctionInterface::create();
}

Int
FunctionInitialCondition::get_num_components() const
{
    _F_;
    return this->num_comps;
}

void
FunctionInitialCondition::evaluate(Int dim, Real time, const Real x[], Int nc, Scalar u[])
{
    _F_;
    evaluate_func(dim, time, x, nc, u);
}

} // namespace godzilla
