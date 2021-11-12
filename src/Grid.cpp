#include "Godzilla.h"
#include "Grid.h"
#include "CallStack.h"

namespace godzilla {

InputParameters
Grid::validParams()
{
    InputParameters params = Object::validParams();
    return params;
}

Grid::Grid(const InputParameters & parameters) : Object(parameters), PrintInterface(this), dm(NULL)
{
    _F_;
}

Grid::~Grid()
{
    _F_;
    if (this->dm)
        DMDestroy(&this->dm);
}

const DM &
Grid::getDM() const
{
    _F_;
    return this->dm;
}

} // namespace godzilla
