#include "grids/GGrid.h"
#include "base/CallStack.h"

InputParameters
GGrid::validParams()
{
    InputParameters params = MooseObject::validParams();
    params.addPrivateParam<std::string>("_moose_base", "GGrid");
    return params;
}

GGrid::GGrid(const InputParameters & parameters) :
    MooseObject(parameters),
    GPrintInterface(this),
    dm(NULL)
{
    _F_;
}

GGrid::~GGrid()
{
    _F_;
    if (this->dm)
        DMDestroy(&this->dm);
}

const DM &
GGrid::getDM() const
{
    return this->dm;
}
