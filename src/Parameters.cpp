// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

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
    for (const auto & [name, value] : rhs)
        this->params[name] = value->copy();
    return *this;
}

} // namespace godzilla
