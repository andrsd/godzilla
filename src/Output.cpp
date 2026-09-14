// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Output.h"
#include "godzilla/CallStack.h"
#include "godzilla/Enums.h"
#include "godzilla/Problem.h"
#include "godzilla/Utils.h"

namespace godzilla {

Parameters
Output::parameters()
{
    auto params = Object::parameters();
    params.add_param<ExecuteOnFlags>("on", "When output should happen")
        .add_param<Int>("interval", "Interval")
        .add_private_param<LateRef<Problem>>("_problem");
    return params;
}

Output::Output(const Parameters & pars) :
    Object(pars),
    PrintInterface(this),
    problem_(pars.get<Ref<Problem>>("_problem")),
    on_mask_(pars.get<ExecuteOnFlags>("on")),
    interval_(pars.get<Int>("interval", 1)),
    last_output_time_(std::nan(""))
{
    CALL_STACK_MSG();
    if (this->on_mask_.has_flags()) {
        if (none_with_flags(this->on_mask_))
            error("The 'none' execution flag can be used only by itself.");
    }
    else
        error("The 'on' parameter can be either 'none' or a combination of 'initial', "
              "'timestep' and/or 'final'.");

    if (pars.is_param_valid("interval") && ((this->on_mask_ & ExecuteOn::TIMESTEP) == 0))
        warning("Parameter 'interval' was specified, but 'on' is missing 'timestep'.");
}

void
Output::create()
{
    CALL_STACK_MSG();
}

void
Output::set_exec_mask(ExecuteOnFlags flags)
{
    CALL_STACK_MSG();
    this->on_mask_ = flags;
}

Ref<Problem>
Output::get_problem() const
{
    CALL_STACK_MSG();
    return this->problem_;
}

ExecuteOnFlags
Output::execute_on() const
{
    CALL_STACK_MSG();
    return this->on_mask_;
}

bool
Output::should_output(ExecuteOn flag)
{
    CALL_STACK_MSG();
    if (this->on_mask_ & flag) {
        bool should;
        if (flag == ExecuteOn::TIMESTEP)
            should = (this->problem_->get_step_num() % this->interval_) == 0;
        else
            should = true;

        constexpr Real TIME_TOL = 1e-12;
        if (should &&
            (std::isnan(this->last_output_time_) ||
             math::abs(this->last_output_time_ - this->problem_->get_time()) > TIME_TOL)) {
            this->last_output_time_ = this->problem_->get_time();
            return true;
        }
        else
            return false;
    }
    else
        return false;
}

} // namespace godzilla
