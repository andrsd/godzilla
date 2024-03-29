// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/FunctionInterface.h"
#include "godzilla/CallStack.h"
#include "godzilla/App.h"
#include "godzilla/Problem.h"
#include <cassert>

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

FunctionInterface::FunctionInterface(const Parameters & params) :
    fi_app(params.get<App *>("_app")),
    expression(params.get<std::vector<std::string>>("value")),
    expression_t(params.has<std::vector<std::string>>("value_t")
                     ? params.get<std::vector<std::string>>("value_t")
                     : std::vector<std::string>())
{
    this->num_comps = this->expression.size();
    assert(this->num_comps >= 1);
}

void
FunctionInterface::create()
{
    CALL_STACK_MSG();
    this->evalr.create(this->expression);
    this->evalr_t.create(this->expression_t);

    assert(this->fi_app != nullptr);
    Problem * p = this->fi_app->get_problem();
    if (p) {
        const auto & funcs = p->get_functions();
        for (auto & f : funcs) {
            this->evalr.register_function(f);
            this->evalr_t.register_function(f);
        }
    }
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
FunctionInterface::evaluate_func(Int dim, Real time, const Real x[], Int nc, Real u[])
{
    CALL_STACK_MSG();
    return this->evalr.evaluate(dim, time, x, nc, u);
}

bool
FunctionInterface::evaluate_func_t(Int dim, Real time, const Real x[], Int nc, Real u[])
{
    CALL_STACK_MSG();
    return this->evalr_t.evaluate(dim, time, x, nc, u);
}

} // namespace godzilla
