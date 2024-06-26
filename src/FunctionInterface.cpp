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
    problem(nullptr),
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
    double t = time;
    auto dim = this->problem->get_dimension();
    std::vector<double> xx(dim);
    for (int i = 0; i < dim; i++)
        xx[i] = x[i];
    std::vector<double> uu(nc);
    for (int i = 0; i < nc; i++)
        uu[i] = u[i];
    return this->evalr.evaluate(this->problem->get_dimension(), t, xx.data(), nc, uu.data());
}

bool
FunctionInterface::evaluate_func_t(Real time, const Real x[], Int nc, Real u[])
{
    CALL_STACK_MSG();
    double t = time;
    auto dim = this->problem->get_dimension();
    std::vector<double> xx(dim);
    for (int i = 0; i < dim; i++)
        xx[i] = x[i];
    std::vector<double> uu(nc);
    for (int i = 0; i < nc; i++)
        uu[i] = u[i];
    return this->evalr_t.evaluate(this->problem->get_dimension(), t, xx.data(), nc, uu.data());
}

} // namespace godzilla
