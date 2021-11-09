#include "Problem.h"
#include "CallStack.h"
#include "GGrid.h"

namespace godzilla {

InputParameters
Problem::validParams()
{
    InputParameters params = Object::validParams();
    params.addPrivateParam<GGrid *>("_ggrid");
    return params;
}

Problem::Problem(const InputParameters & parameters) :
    Object(parameters),
    GPrintInterface(this),
    grid(*getParam<GGrid *>("_ggrid"))
{
    _F_;
}

}
