#include "Godzilla.h"
#include "Executioner.h"
#include "Problem.h"
#include "CallStack.h"
#include "Output.h"


namespace godzilla {

registerObject(Executioner);

InputParameters
Executioner::validParams()
{
    InputParameters params = Object::validParams();
    params.addPrivateParam<Problem *>("_Problem");
    return params;
}

Executioner::Executioner(const InputParameters & parameters) :
    Object(parameters),
    GPrintInterface(this),
    problem(*getParam<Problem *>("_Problem"))
{
    _F_;
}

void
Executioner::create()
{
    _F_;
}

void
Executioner::execute()
{
    _F_;
    godzillaPrint(5, "Executing...");

    this->problem.solve();
    output();
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

}
