#include "GProblem.h"
#include "CallStack.h"
#include "GGrid.h"

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
