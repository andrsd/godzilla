#include "PiecewiseLinear.h"
#include "Godzilla.h"
#include "CallStack.h"

namespace godzilla {

REGISTER_OBJECT(PiecewiseLinear);

double
__piecewise_linear_function_eval(void * ctx, double x)
{
    PiecewiseLinear * fn = static_cast<PiecewiseLinear *>(ctx);
    return fn->evaluate(x);
}

Parameters
PiecewiseLinear::valid_params()
{
    Parameters params = Function::valid_params();
    params.add_required_param<std::vector<PetscReal>>("x", "Independent variable");
    params.add_required_param<std::vector<PetscReal>>("y", "Dependent variable");
    return params;
}

PiecewiseLinear::PiecewiseLinear(const Parameters & params) :
    Function(params),
    linpol(get_param<std::vector<PetscReal>>("x"), get_param<std::vector<PetscReal>>("y"))
{
    _F_;
}

void
PiecewiseLinear::register_callback(mu::Parser & parser)
{
    _F_;
    parser.DefineFunUserData(get_name(), __piecewise_linear_function_eval, this);
}

PetscReal
PiecewiseLinear::evaluate(PetscReal x)
{
    _F_;
    return this->linpol.sample(x);
}

} // namespace godzilla
