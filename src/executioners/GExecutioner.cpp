#include "GExecutioner.h"
#include "GProblem.h"

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
}

void
GExecutioner::execute()
{
    godzillaPrint(5, "Executing...");

    problem.solve();
}
