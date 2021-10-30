#include "executioners/GExecutioner.h"
#include "problems/GProblem.h"
#include "base/CallStack.h"

registerMooseObject("GodzillaApp", GExecutioner);

InputParameters
GExecutioner::validParams()
{
    InputParameters params = MooseObject::validParams();
    params.addPrivateParam<const GodzillaApp *>("_gapp");
    params.addPrivateParam<GProblem *>("_gproblem");
    params.addPrivateParam<std::string>("_moose_base", "gexecutioner");
    return params;
}

GExecutioner::GExecutioner(const InputParameters & parameters) :
    MooseObject(parameters),
    GPrintInterface(*getParam<const GodzillaApp *>("_gapp")),
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

    problem.solve();
}
