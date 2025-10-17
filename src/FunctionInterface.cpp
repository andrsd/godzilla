// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/FunctionInterface.h"
#include "godzilla/CallStack.h"
#include "godzilla/App.h"
#include "godzilla/Problem.h"

namespace godzilla {

Parameters
FunctionInterface::parameters()
{
    Parameters params;
    params.add_required_param<std::vector<std::string>>("value",
                                                        "Function expression to evaluate.");
    return params;
}

Parameters
FunctionInterface::valid_params_t()
{
    Parameters params;
    params.add_param<std::vector<std::string>>("value_t",
                                               "Time derivative of the 'value' parameter.");
    return params;
}

FunctionInterface::FunctionInterface(const Parameters & pars) :
    fi_app(pars.get<App *>("_app")),
    problem(nullptr),
    expression(pars.get<std::vector<std::string>>("value")),
    expression_t(pars.is_param_valid("value_t") ? pars.get<std::vector<std::string>>("value_t")
                                                : std::vector<std::string>())
{
    this->num_comps = this->expression.size();
    // TODO: turn this into `log_error`
    if (this->num_comps == 0)
        throw Exception("No components provided");
}

void
FunctionInterface::create()
{
    CALL_STACK_MSG();
    this->evalr.create(this->expression);
    this->evalr_t.create(this->expression_t);

    this->problem = this->fi_app != nullptr ? this->fi_app->get_problem() : nullptr;
    if (this->problem) {
        const auto & funcs = this->problem->get_functions();
        for (auto & f : funcs) {
            this->evalr.register_function(f);
            this->evalr_t.register_function(f);
        }
    }
    else
        throw Exception("No `Problem` associated with the function");
}

unsigned int
FunctionInterface::get_num_components() const
{
    return this->num_comps;
}

bool
FunctionInterface::has_time_expression() const
{
    return !this->expression_t.empty();
}

bool
FunctionInterface::evaluate_func(Real time, const Real x[], Int nc, Real u[])
{
    CALL_STACK_MSG();
    return this->evalr.evaluate(this->problem->get_dimension(), time, x, nc, u);
}

bool
FunctionInterface::evaluate_func_t(Real time, const Real x[], Int nc, Real u[])
{
    CALL_STACK_MSG();
    return this->evalr_t.evaluate(this->problem->get_dimension(), time, x, nc, u);
}

} // namespace godzilla
