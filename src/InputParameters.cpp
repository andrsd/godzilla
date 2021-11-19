#include "InputParameters.h"
#include <cmath>

namespace godzilla {

InputParameters
emptyInputParameters()
{
    InputParameters params;
    return params;
}

InputParameters::InputParameters() {}

InputParameters::InputParameters(const InputParameters & p)
{
    *this = p;
}

InputParameters::~InputParameters()
{
    clear();
}

InputParameters &
InputParameters::operator=(const InputParameters & rhs)
{
    this->clear();
    for (auto it = rhs.begin(); it != rhs.end(); ++it)
        this->params[it->first] = it->second->copy();
    return *this;
}

} // namespace godzilla
