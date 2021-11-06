#include "problems/GProblem.h"
#include "base/CallStack.h"
#include "grids/GGrid.h"

namespace godzilla {

InputParameters
GProblem::validParams()
{
    InputParameters params = Object::validParams();
    params.addPrivateParam<GGrid *>("_ggrid");
    return params;
}

GProblem::GProblem(const InputParameters & parameters) :
    Object(parameters),
    GPrintInterface(this),
    grid(*getParam<GGrid *>("_ggrid"))
{
    _F_;
}

}
