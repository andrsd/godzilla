#include "Parameters.h"
#include <cmath>

namespace godzilla {

Parameters::Parameters() {}

Parameters::Parameters(const Parameters & p)
{
    *this = p;
}

Parameters::~Parameters()
{
    clear();
}

Parameters &
Parameters::operator=(const Parameters & rhs)
{
    this->clear();
    for (auto it = rhs.begin(); it != rhs.end(); ++it)
        this->params[it->first] = it->second->copy();
    return *this;
}

} // namespace godzilla
