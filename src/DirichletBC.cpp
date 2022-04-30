#include "Godzilla.h"
#include "DirichletBC.h"

namespace godzilla {

registerObject(DirichletBC);

InputParameters
DirichletBC::validParams()
{
    InputParameters params = EssentialBC::validParams();
    params += ParsedFunctionInterface::validParams();
    return params;
}

DirichletBC::DirichletBC(const InputParameters & params) :
    EssentialBC(params),
    ParsedFunctionInterface(params)
{
    _F_;
}

void
DirichletBC::create()
{
    _F_;
    ParsedFunctionInterface::create();
}

Scalar
DirichletBC::evaluate(Real time, Real x, Real y, Real z)
{
    _F_;
    // FIXME: set this to the dimension of the FEProblem
    uint dim = 1;
    return evaluateFunction(0, dim, time, x, y, z);
}

} // namespace godzilla
