#include "problems/GProblem.h"
#include "base/CallStack.h"

InputParameters
GProblem::validParams()
{
    InputParameters params = Problem::validParams();
    params.addPrivateParam<std::string>("_moose_base", "gproblem");
    return params;
}

GProblem::GProblem(const InputParameters & parameters) :
    Problem(parameters),
    GPrintInterface(this)
{
    _F_;
}

void
GProblem::init()
{
    _F_;
}
