#include "Problem.h"
#include "CallStack.h"
#include "Grid.h"

namespace godzilla {

InputParameters
Problem::validParams()
{
    InputParameters params = Object::validParams();
    params.addPrivateParam<Grid *>("_grid");
    return params;
}

Problem::Problem(const InputParameters & parameters) :
    Object(parameters),
    PrintInterface(this),
    grid(*getParam<Grid *>("_grid"))
{
    _F_;
}

void
Problem::addOutput(const Output * output)
{
    _F_;
    this->outputs.push_back(output);
}

} // namespace godzilla
