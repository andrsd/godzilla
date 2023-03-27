#include "PiecewiseLinear.h"
#include "Godzilla.h"
#include "CallStack.h"

namespace godzilla {

REGISTER_OBJECT(PiecewiseLinear);

static double
piecewise_linear_function_eval(void * ctx, double x)
{
    auto * fn = static_cast<PiecewiseLinear *>(ctx);
    return fn->evaluate(x);
}

Parameters
PiecewiseLinear::parameters()
{
    Parameters params = Function::parameters();
    params.add_required_param<std::vector<Real>>("x", "Independent variable");
    params.add_required_param<std::vector<Real>>("y", "Dependent variable");
    return params;
}

PiecewiseLinear::PiecewiseLinear(const Parameters & params) :
    Function(params),
    linpol(get_param<std::vector<Real>>("x"), get_param<std::vector<Real>>("y"))
{
    _F_;
}

void
PiecewiseLinear::register_callback(mu::Parser & parser)
{
    _F_;
    parser.DefineFunUserData(get_name(), piecewise_linear_function_eval, this);
}

void
PiecewiseLinear::check()
{
    _F_;
    Function::check();
    try {
        this->linpol.check();
    }
    catch (std::exception & e) {
        log_error(e.what());
    }
}

Real
PiecewiseLinear::evaluate(Real x)
{
    _F_;
    return this->linpol.sample(x);
}

} // namespace godzilla
