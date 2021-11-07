#include "Godzilla.h"
#include "GExecutioner.h"
#include "GProblem.h"
#include "CallStack.h"
#include "GOutput.h"


namespace godzilla {

registerObject(GExecutioner);

InputParameters
GExecutioner::validParams()
{
    InputParameters params = Object::validParams();
    params.addPrivateParam<GProblem *>("_gproblem");
    return params;
}

GExecutioner::GExecutioner(const InputParameters & parameters) :
    Object(parameters),
    GPrintInterface(this),
    problem(*getParam<GProblem *>("_gproblem"))
{
    _F_;
}

void
GExecutioner::create()
{
    _F_;
}

void
GExecutioner::execute()
{
    _F_;
    godzillaPrint(5, "Executing...");

    this->problem.solve();
    output();
}

void
GExecutioner::addOutput(const GOutput * output)
{
    _F_;
    this->outputs.push_back(output);
}

void
GExecutioner::output()
{
    _F_;
    for (auto & o : this->outputs)
        o->output();
}

}
