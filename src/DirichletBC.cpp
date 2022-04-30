#include "Godzilla.h"
#include "DirichletBC.h"

namespace godzilla {

registerObject(DirichletBC);

InputParameters
DirichletBC::validParams()
{
    InputParameters params = EssentialBC::validParams();
    return params;
}

DirichletBC::DirichletBC(const InputParameters & params) :
    EssentialBC(params)
{
    _F_;
}

void
DirichletBC::create()
{
    _F_;
}

Scalar
DirichletBC::evaluate(Real x, Real y, Real z) const
{
    _F_;
    return x * x;
}

} // namespace godzilla
