#include "Godzilla.h"
#include "grids/GGrid.h"
#include "base/CallStack.h"


namespace godzilla {

InputParameters
GGrid::validParams()
{
    InputParameters params = Object::validParams();
    return params;
}

GGrid::GGrid(const InputParameters & parameters) :
    Object(parameters),
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

}
