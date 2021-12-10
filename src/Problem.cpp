#include "Problem.h"
#include "CallStack.h"
#include "Grid.h"
#include "Output.h"

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
}

Problem::~Problem() {}

void
Problem::check()
{
    _F_;
    for (auto & out : this->outputs) {
        out->check();
    }
}

void
Problem::create()
{
    _F_;
    for (auto & out : this->outputs)
        out->create();
}

void
Problem::addOutput(Output * output)
{
    _F_;
    this->outputs.push_back(output);
}

} // namespace godzilla
