#include "problems/GProblem.h"
#include "base/CallStack.h"


namespace godzilla {

InputParameters
GProblem::validParams()
{
    InputParameters params = Object::validParams();
    return params;
}

GProblem::GProblem(const InputParameters & parameters) :
    Object(parameters),
    GPrintInterface(this)
{
    _F_;
}

}
