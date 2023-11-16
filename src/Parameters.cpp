#include "godzilla/Parameters.h"

namespace godzilla {

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
    for (const auto & par : rhs)
        this->params[par.first] = par.second->copy();
    return *this;
}

} // namespace godzilla
