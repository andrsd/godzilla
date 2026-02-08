// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Postprocessor.h"
#include "godzilla/Problem.h"

namespace godzilla {

Parameters
Postprocessor::parameters()
{
    auto params = Object::parameters();
    params.add_private_param<LateRef<Problem>>("_problem");
    return params;
}

Postprocessor::Postprocessor(const Parameters & pars) :
    Object(pars),
    PrintInterface(this),
    problem(pars.get<Ref<Problem>>("_problem"))
{
}

Ref<Problem>
Postprocessor::get_problem() const
{
    CALL_STACK_MSG();
    return this->problem;
}

} // namespace godzilla
