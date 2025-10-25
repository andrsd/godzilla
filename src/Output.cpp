// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Output.h"
#include "godzilla/CallStack.h"
#include "godzilla/Enums.h"
#include "godzilla/Problem.h"
#include "godzilla/Utils.h"

namespace godzilla {

namespace {

bool
none_with_flags(const ExecuteOn & flags)
{
    auto mask = flags.get_mask();
    auto none_flag = ExecuteOnFlag::EXECUTE_ON_NONE;
    return (flags & none_flag) && ((mask & ~none_flag) != 0);
}

} // namespace

Parameters
Output::parameters()
{
    auto params = Object::parameters();
    params.add_param<ExecuteOn>("on", "When output should happen")
        .add_param<Int>("interval", "Interval")
        .add_private_param<Problem *>("_problem");
    return params;
}

Output::Output(const Parameters & pars) :
    Object(pars),
    PrintInterface(this),
    problem(pars.get<Problem *>("_problem")),
    interval(pars.get<Int>("interval", 1)),
    last_output_time(std::nan(""))
{
    CALL_STACK_MSG();
    if (pars.is_param_valid("on")) {
        const auto on = pars.get<ExecuteOn>("on");
        if (on.has_flags()) {
            if (none_with_flags(on))
                log_error("The 'none' execution flag can be used only by itself.");
            else
                this->on_mask = on;
        }
        else
            log_error("The 'on' parameter can be either 'none' or a combination of 'initial', "
                      "'timestep' and/or 'final'.");
    }
    else
        this->on_mask = this->problem->get_default_output_on();

    if (pars.is_param_valid("interval") && ((this->on_mask & EXECUTE_ON_TIMESTEP) == 0))
        log_warning("Parameter 'interval' was specified, but 'on' is missing 'timestep'.");
}

void
Output::create()
{
    CALL_STACK_MSG();
}

void
Output::set_exec_mask(ExecuteOn mask)
{
    CALL_STACK_MSG();
    this->on_mask = mask;
}

Problem *
Output::get_problem() const
{
    CALL_STACK_MSG();
    return this->problem;
}

ExecuteOn
Output::get_exec_mask() const
{
    CALL_STACK_MSG();
    return this->on_mask;
}

ExecuteOn
Output::execute_on() const
{
    CALL_STACK_MSG();
    return this->on_mask;
}

bool
Output::should_output(ExecuteOnFlag flag)
{
    CALL_STACK_MSG();
    if (this->on_mask & flag) {
        bool should;
        if (flag == EXECUTE_ON_TIMESTEP)
            should = (this->problem->get_step_num() % this->interval) == 0;
        else
            should = true;

        constexpr Real TIME_TOL = 1e-12;
        if (should && (std::isnan(this->last_output_time) ||
                       math::abs(this->last_output_time - this->problem->get_time()) > TIME_TOL)) {
            this->last_output_time = this->problem->get_time();
            return true;
        }
        else
            return false;
    }
    else
        return false;
}

} // namespace godzilla
