// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Postprocessor.h"
#include "godzilla/Problem.h"

namespace godzilla {

Parameters
Postprocessor::parameters()
{
    Parameters params = Object::parameters();
    params.add_private_param<Problem *>("_problem", nullptr);
    return params;
}

Postprocessor::Postprocessor(const Parameters & params) :
    Object(params),
    PrintInterface(this),
    problem(get_param<Problem *>("_problem"))
{
}

Problem *
Postprocessor::get_problem() const
{
    _F_;
    return this->problem;
}

} // namespace godzilla
