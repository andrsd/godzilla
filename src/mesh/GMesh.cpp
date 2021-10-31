#include "mesh/GMesh.h"
#include "base/CallStack.h"

InputParameters
GMesh::validParams()
{
    InputParameters params = MooseObject::validParams();
    params.addPrivateParam<std::string>("_moose_base", "gmesh");
    return params;
}

GMesh::GMesh(const InputParameters & parameters) :
    MooseObject(parameters),
    GPrintInterface(this)
{
    _F_;
}

GMesh::~GMesh()
{
    _F_;
    DMDestroy(&this->dm);
}

const DM &
GMesh::getDM() const
{
    return this->dm;
}
