// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Output.h"
#include "godzilla/CallStack.h"
#include "godzilla/Problem.h"
#include "godzilla/Utils.h"

namespace godzilla {

Parameters
Output::parameters()
{
    Parameters params = Object::parameters();
    params.add_param<std::vector<std::string>>("on", "When output should happen");
    params.add_param<Int>("interval", "Interval");
    params.add_private_param<Problem *>("_problem", nullptr);
    return params;
}

Output::Output(const Parameters & params) :
    Object(params),
    PrintInterface(this),
    problem(get_param<Problem *>("_problem")),
    on_mask(),
    interval(is_param_valid("interval") ? get_param<Int>("interval") : 1),
    last_output_time(std::nan(""))
{
    CALL_STACK_MSG();
}

void
Output::create()
{
    CALL_STACK_MSG();
    set_up_exec();
    if (is_param_valid("interval") && ((this->on_mask & EXECUTE_ON_TIMESTEP) == 0))
        log_warning("Parameter 'interval' was specified, but 'on' is missing 'timestep'.");
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

void
Output::set_up_exec()
{
    CALL_STACK_MSG();
    if (is_param_valid("on")) {
        const auto & on = get_param<std::vector<std::string>>("on");
        if (!on.empty()) {
            bool none = false;
            ExecuteOn mask;
            for (auto & s : on) {
                std::string ls = utils::to_lower(s);
                if (ls == "initial")
                    mask |= EXECUTE_ON_INITIAL;
                else if (ls == "timestep")
                    mask |= EXECUTE_ON_TIMESTEP;
                else if (ls == "final")
                    mask |= EXECUTE_ON_FINAL;
                else if (ls == "none")
                    none = true;
            }

            if (none && (mask.has_flags()))
                log_error("The 'none' execution flag can be used only by itself.");
            else
                this->on_mask = mask;
        }
        else
            log_error("The 'on' parameter can be either 'none' or a combination of 'initial', "
                      "'timestep' and/or 'final'.");
    }
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
                       std::abs(this->last_output_time - this->problem->get_time()) > TIME_TOL)) {
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
