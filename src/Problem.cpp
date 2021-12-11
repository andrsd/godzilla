#include "Problem.h"
#include "CallStack.h"
#include "Grid.h"
#include "Postprocessor.h"
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
    for (auto & pp : this->pps)
        pp->check();
    for (auto & out : this->outputs) {
        out->check();
    }
}

void
Problem::create()
{
    _F_;
    for (auto & pp : this->pps)
        pp->create();
    for (auto & out : this->outputs)
        out->create();
}

void
Problem::addOutput(Output * output)
{
    _F_;
    this->outputs.push_back(output);
}

void
Problem::addPostprocessor(Postprocessor * pp)
{
    _F_;
    this->pps.push_back(pp);
}

void
Problem::computePostprocessors()
{
    _F_;
    for (auto & pp : this->pps)
        pp->compute();
}

} // namespace godzilla
