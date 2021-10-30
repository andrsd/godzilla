#include "mesh/GMesh.h"
#include "base/CallStack.h"

registerMooseObject("GodzillaApp", GMesh);

InputParameters
GMesh::validParams()
{
    InputParameters params = MooseObject::validParams();
    params.addPrivateParam<const GodzillaApp *>("_gapp");
    params.addPrivateParam<std::string>("_moose_base", "gmesh");
    return params;
}

GMesh::GMesh(const InputParameters & parameters) :
    MooseObject(parameters),
    GPrintInterface(*getParam<const GodzillaApp *>("_gapp"))
{
    _F_
}
