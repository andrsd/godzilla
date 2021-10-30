#include "GProblem.h"

registerMooseObject("GodzillaApp", GProblem);

InputParameters
GProblem::validParams()
{
    InputParameters params = Problem::validParams();
    params.addPrivateParam<const GodzillaApp *>("_gapp");
    params.addPrivateParam<GMesh *>("_gmesh");
    params.set<std::string>("_moose_base") = "gproblem";
    return params;
}

GProblem::GProblem(const InputParameters & parameters) :
    Problem(parameters),
    GPrintInterface(*getParam<const GodzillaApp *>("_gapp")),
    _mesh(*getParam<GMesh *>("_gmesh"))
{
}

void
GProblem::init()
{
    godzillaPrint(5, "Initialize...");
}

void
GProblem::solve()
{
    godzillaPrint(5, "Solving...");
}

bool
GProblem::converged()
{
    return false;
}
