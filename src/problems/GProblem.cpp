#include "problems/GProblem.h"
#include "base/CallStack.h"

registerMooseObject("GodzillaApp", GProblem);

InputParameters
GProblem::validParams()
{
    InputParameters params = Problem::validParams();
    params.addPrivateParam<const GodzillaApp *>("_gapp");
    params.addPrivateParam<GMesh *>("_gmesh");
    params.addPrivateParam<std::string>("_moose_base", "gproblem");
    return params;
}

GProblem::GProblem(const InputParameters & parameters) :
    Problem(parameters),
    GPrintInterface(*getParam<const GodzillaApp *>("_gapp")),
    mesh(*getParam<GMesh *>("_gmesh"))
{
    _F_;
}

void
GProblem::create()
{
    _F_;
}

void
GProblem::init()
{
    _F_;
}

void
GProblem::solve()
{
    _F_;
    godzillaPrint(5, "Solving...");
}

bool
GProblem::converged()
{
    _F_;
    return false;
}
