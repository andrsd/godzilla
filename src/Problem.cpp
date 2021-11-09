#include "Problem.h"
#include "CallStack.h"
#include "Grid.h"

namespace godzilla {

InputParameters
Problem::validParams()
{
    InputParameters params = Object::validParams();
    params.addPrivateParam<Grid *>("_ggrid");
    return params;
}

Problem::Problem(const InputParameters & parameters) :
    Object(parameters),
    PrintInterface(this),
    grid(*getParam<Grid *>("_ggrid"))
{
    _F_;
}

}
