#include "PiecewiseLinear.h"
#include "Godzilla.h"

namespace godzilla {

registerObject(PiecewiseLinear);

double
__piecewise_linear_function_eval(void * ctx, double x)
{
    PiecewiseLinear * fn = static_cast<PiecewiseLinear *>(ctx);
    return fn->evaluate(x);
}

InputParameters
PiecewiseLinear::validParams()
{
    InputParameters params = Function::validParams();
    params.addRequiredParam<std::vector<PetscReal>>("x", "Independent variable");
    params.addRequiredParam<std::vector<PetscReal>>("y", "Dependent variable");
    return params;
}

PiecewiseLinear::PiecewiseLinear(const InputParameters & params) :
    Function(params),
    linpol(getParam<std::vector<PetscReal>>("x"), getParam<std::vector<PetscReal>>("y"))
{
    _F_;
}

void
PiecewiseLinear::registerCallback(mu::Parser & parser)
{
    _F_;
    parser.DefineFunUserData(getName(), __piecewise_linear_function_eval, this);
}

PetscReal
PiecewiseLinear::evaluate(PetscReal x)
{
    _F_;
    return this->linpol.sample(x);
}

} // namespace godzilla
