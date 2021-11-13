#include "Godzilla.h"
#include "Executioner.h"
#include "Problem.h"
#include "CallStack.h"
#include "Output.h"

namespace godzilla {

InputParameters
Executioner::validParams()
{
    InputParameters params = Object::validParams();
    params.addPrivateParam<Problem *>("_problem");
    return params;
}

Executioner::Executioner(const InputParameters & parameters) :
    Object(parameters),
    PrintInterface(this),
    problem(*getParam<Problem *>("_problem"))
{
    _F_;
}

void
Executioner::create()
{
    _F_;
}

void
Executioner::addOutput(const Output * output)
{
    _F_;
    this->outputs.push_back(output);
}

void
Executioner::output()
{
    _F_;
    for (auto & o : this->outputs)
        o->output();
}

} // namespace godzilla
