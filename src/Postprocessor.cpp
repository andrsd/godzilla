// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Postprocessor.h"
#include "godzilla/CallStack.h"
#include "godzilla/Problem.h"
#include "godzilla/Utils.h"

namespace godzilla {

Parameters
Postprocessor::parameters()
{
    auto params = Object::parameters();
    params.add_private_param<LateRef<Problem>>("_problem")
        .add_param<ExecuteOnFlags>("on", "When postprocessor should be executed");
    return params;
}

Postprocessor::Postprocessor(const Parameters & pars) :
    Object(pars),
    PrintInterface(this),
    problem(pars.get<Ref<Problem>>("_problem")),
    on_mask(pars.get<ExecuteOnFlags>("on")),
    last_execute_time(std::nan(""))
{
    CALL_STACK_MSG();
    if (this->on_mask.has_flags()) {
        if (none_with_flags(this->on_mask))
            error("The 'none' execution flag can be used only by itself.");
    }
    else
        error("The 'on' parameter can be either 'none' or a combination of 'initial', "
              "'timestep' and/or 'final'.");
}

ExecuteOnFlags
Postprocessor::execute_on() const
{
    CALL_STACK_MSG();
    return this->on_mask;
}

Ref<Problem>
Postprocessor::get_problem() const
{
    CALL_STACK_MSG();
    return this->problem;
}

bool
Postprocessor::should_execute(ExecuteOn flag)
{
    CALL_STACK_MSG();
    if (this->on_mask & flag) {
        constexpr Real TIME_TOL = 1e-12;
        if ((std::isnan(this->last_execute_time) ||
             math::abs(this->last_execute_time - this->problem->get_time()) > TIME_TOL)) {
            this->last_execute_time = this->problem->get_time();
            return true;
        }
        else
            return false;
    }
    else
        return false;
}

} // namespace godzilla
